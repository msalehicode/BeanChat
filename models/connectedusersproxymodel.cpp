#include "connectedusersproxymodel.h"

ConnectedUsersProxyModel::ConnectedUsersProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

bool ConnectedUsersProxyModel::lessThan(
    const QModelIndex &left,
    const QModelIndex &right) const
{
    int leftStatus =
        sourceModel()->data(
                         left,
                         ConnectedUsersModel::UserStatusRole).toInt();

    int rightStatus =
        sourceModel()->data(
                         right,
                         ConnectedUsersModel::UserStatusRole).toInt();

    int lp = statusPriority(leftStatus);
    int rp = statusPriority(rightStatus);

    if(lp != rp)
        return lp < rp;

    QString leftName =
        sourceModel()->data(
                         left,
                         ConnectedUsersModel::UserNameRole).toString();

    QString rightName =
        sourceModel()->data(
                         right,
                         ConnectedUsersModel::UserNameRole).toString();

    return leftName.compare(
               rightName,
               Qt::CaseInsensitive) < 0;
}

int ConnectedUsersProxyModel::statusPriority(int status)
{
    switch(status)
    {
    case BeanChatCommon::Presence::Online: return 0;
    case BeanChatCommon::Presence::Busy: return 1;
    case BeanChatCommon::Presence::DoNotDisturb: return 2;
    case BeanChatCommon::Presence::Away: return 3;
    case BeanChatCommon::Presence::Idle: return 4;
    case BeanChatCommon::Presence::Offline: return 5;
    default: return 6;
    }
}
