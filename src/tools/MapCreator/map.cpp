#include "map.h"
#include <QDebug>

StaticMap::StaticMap()
{

}

StaticMap::StaticMap(int row, int column)
{
    qDebug() << "row = " << row;
    qDebug() << "column =" << column;
    items.resize(row);

    for( auto & x:items )
    {
        x.resize(column);
    }
}
