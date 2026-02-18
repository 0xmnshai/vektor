#include "VKE_list_base.h"

namespace vektor
{

void VKE_addtail(ListBase* listbase,
                 void*     vlink)
{
    Link* link = static_cast<Link*>(vlink);

    if (link == nullptr)
    {
        return;
    }

    link->next = nullptr;
    link->prev = static_cast<Link*>(listbase->last);

    if (listbase->last)
    {
        (static_cast<Link*>(listbase->last))->next = link;
    }
    if (listbase->first == nullptr)
    {
        listbase->first = link;
    }
    listbase->last = link;
}
} // namespace vektor