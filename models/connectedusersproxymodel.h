#pragma once

#include <QSortFilterProxyModel>
#include "connectedusersmodel.h"
#include <protocol/commonTypes.h>

class ConnectedUsersProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ConnectedUsersProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(
        const QModelIndex &left,
        const QModelIndex &right) const override;

private:
    static int statusPriority(int status);
};
