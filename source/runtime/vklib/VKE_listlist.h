#pragma once

namespace vektor
{
struct LinkNode
{
    struct LinkNode* next;
    void*            link;
};

} // namespace vektor