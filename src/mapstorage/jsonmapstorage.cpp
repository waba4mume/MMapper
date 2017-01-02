/************************************************************************
**
** Authors:   Ulf Hermann <ulfonk_mennhar@gmx.de> (Alve),
**            Marek Krejza <krejza@gmail.com> (Caligor),
**            Nils Schimmelmann <nschimme@gmail.com> (Jahara)
**
** This file is part of the MMapper project.
** Maintained by Nils Schimmelmann <nschimme@gmail.com>
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the:
** Free Software Foundation, Inc.
** 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
************************************************************************/

#include "jsonmapstorage.h"
#include "mapdata.h"
#include "roomsaver.h"
#include "patterns.h"
#include "configuration.h"
#include "oldconnection.h"
#include "oldroom.h"
#include "mmapper2room.h"
#include "mmapper2exit.h"
#include "progresscounter.h"
#include "basemapsavefilter.h"
#include "infomark.h"
#include "qtiocompressor.h"
#include "olddoor.h"

#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMap>

#include <cassert>
#include <iostream>

using namespace std;

/* JsonRoomIdsCache: This is room for possibly indexing the rooms by name &
 * desc later. For now we use the internal MM2 ID. The JsonId needs to be short
 * enough to be referenced in room exits. NB: there *are* collisions in
 * name+descs. */

namespace
{
QString getJsonId(const Room *room)
{
  return QString::number(room->getId());
}
}

class JsonRoomIdsCache
{
  typedef QMap<uint, QString> CacheT;
  CacheT m_cache;

public:
  void buildCache(QList<const Room *> rooms);
  QString operator[](uint roomId) const;
};

void JsonRoomIdsCache::buildCache(QList<const Room *> roomList)
{
  QListIterator<const Room *> roomit(roomList);
  while (roomit.hasNext())
  {
    const Room *room = roomit.next();
    // NB: catch collisions if using some kind of checksum here
    m_cache[room->getId()] = getJsonId(room);
  }
}

QString JsonRoomIdsCache::operator[](uint roomId) const
{
  CacheT::const_iterator it = m_cache.find(roomId);
  assert(it != m_cache.end());
  return *it;
}





JsonMapStorage::JsonMapStorage(MapData& mapdata, const QString& filename, QFile* file) :
    AbstractMapStorage(mapdata, filename, file)
{}

JsonMapStorage::JsonMapStorage(MapData& mapdata, const QString& filename) :
    AbstractMapStorage(mapdata, filename)
{}

JsonMapStorage::~JsonMapStorage()
{
}

void JsonMapStorage::newData()
{
  assert("JsonMapStorage does not implement newData()" != NULL);
}

bool JsonMapStorage::loadData()
{
  return false;
}

bool JsonMapStorage::mergeData()
{
  return false;
}

void JsonMapStorage::saveRoom(const Room * room, QJsonObject &jRooms, const JsonRoomIdsCache &jRoomIds)
{
  /*
        x: 5, y: 5, z: 0,
        north: null, east: 1, south: null, west: null, up: null, down: null,
        sector: 2 * SECT_CITY *, mobflags: 0, loadflags: 0, light: null, RIDEABLE: null,
        name: "Fortune's Delving",
        desc:
        "A largely ceremonial hall, it was the first mineshaft that led down to what is\n"
  */

  const Coordinate & pos = room->getPosition();
  QJsonObject jr;
  jr["x"] = pos.x;
  jr["y"] = pos.y;
  jr["z"] = pos.z;

  QString jsonId = jRoomIds[room->getId()];
  jr["id"]        = jsonId;
  jr["name"]      = getName(room);
  jr["desc"]      = getDescription(room);
  jr["sector"]    = (quint8)getTerrainType(room);
  jr["light"]     = (quint8)getLightType(room);
  jr["portable"]  = (quint8)getPortableType(room);
  jr["rideable"]  = (quint8)getRidableType(room);
  jr["mobflags"]  = (quint16)getMobFlags(room);
  jr["loadflags"] = (quint16)getLoadFlags(room);

  saveExits(room, jr, jRoomIds);

  jRooms[jsonId] = jr;
}

void JsonMapStorage::saveExits(const Room * room, QJsonObject &jr, const JsonRoomIdsCache &jRoomIds)
{
  const ExitsList& exitList = room->getExitsList();
  ExitsListIterator el(exitList);
  QJsonArray jExits; // Direction-indexed
  while (el.hasNext())
  {
    const Exit & e = el.next();
    QJsonObject je;
    je["flags"]  = getFlags(e);
    je["dflags"] = getDoorFlags(e);
    je["name"]   = getDoorName(e);

    QJsonArray jin;
    for (set<uint>::const_iterator i = e.inBegin(); i != e.inEnd(); ++i)
    {
      jin << jRoomIds[*i];
    }
    je["in"] = jin;

    QJsonArray jout;
    for (set<uint>::const_iterator i = e.outBegin(); i != e.outEnd(); ++i)
    {
      jout << jRoomIds[*i];
    }
    je["out"] = jout;

    jExits << je;
  }

  jr["exits"] = jExits;
}

bool JsonMapStorage::saveData( bool baseMapOnly )
{
  emit log ("JsonMapStorage", "Writing data to file ...");

  QTextStream stream (m_file);

  // Collect the room and marker lists. The room list can't be acquired
  // directly apparently and we have to go through a RoomSaver which receives
  // them from a sort of callback function.
  QList<const Room *> roomList;
  MarkerList& markerList = m_mapData.getMarkersList();
  RoomSaver saver(&m_mapData, roomList);
  for (uint i = 0; i < m_mapData.getRoomsCount(); ++i)
  {
    m_mapData.lookingForRooms(&saver, i);
  }

  uint roomsCount = saver.getRoomsCount();
  uint marksCount = markerList.size();
  m_progressCounter->reset();
  m_progressCounter->increaseTotalStepsBy( roomsCount + marksCount );

  BaseMapSaveFilter filter;
  if ( baseMapOnly )
  {
      filter.setMapData( &m_mapData );
      m_progressCounter->increaseTotalStepsBy( filter.prepareCount() );
      filter.prepare( m_progressCounter );
  }

  JsonRoomIdsCache jRoomIds;
  jRoomIds.buildCache(roomList);

  QJsonObject jRooms;

  // save rooms
  unsigned filtered = 0, saved = 0;
  QListIterator<const Room *> roomit(roomList);
  while (roomit.hasNext())
  {
    const Room *room = roomit.next();
    if ( baseMapOnly )
    {
      BaseMapSaveFilter::Action action = filter.filter( room );
      if ( !room->isTemporary() && action != BaseMapSaveFilter::REJECT )
      {
        if ( action == BaseMapSaveFilter::ALTER )
        {
          Room copy = filter.alteredRoom( room );
          saveRoom( &copy, jRooms, jRoomIds );
        }
        else // action == PASS
        {
          saveRoom(room, jRooms, jRoomIds);
        }
        ++saved;
      }
      else
        ++filtered;
    }
    else
    {
      saveRoom(room, jRooms, jRoomIds);
        ++saved;
    }

    m_progressCounter->step();
  }
  emit log ("JsonMapStorage", tr("%1 rooms saved, %2 rooms filtered out.").arg(saved).arg(filtered));

  stream << QJsonDocument(jRooms).toJson();
  stream.flush();

  emit log ("JsonMapStorage", "Writing data finished.");

  m_mapData.unsetDataChanged();
  emit onDataSaved();

  return true;
}


void JsonMapStorage::saveMark(InfoMark * mark, QJsonObject &jRooms, const JsonRoomIdsCache &jRoomIds)
{
  /*
  stream << (QString)mark->getName();
  stream << (QString)mark->getText();
  stream << (QDateTime)mark->getTimeStamp();
  stream << (quint8)mark->getType();
  stream << (quint8)mark->getClass();
  stream << (qint32)(mark->getRotationAngle()*100);
  const Coordinate & c1 = mark->getPosition1();
  const Coordinate & c2 = mark->getPosition2();
  stream << (qint32)c1.x;
  stream << (qint32)c1.y;
  stream << (qint32)c1.z;
  stream << (qint32)c2.x;
  stream << (qint32)c2.y;
  stream << (qint32)c2.z;
  */
}
