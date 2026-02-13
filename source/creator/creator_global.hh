#ifndef __CREATOR_GLOBAL_H__
#define __CREATOR_GLOBAL_H__

namespace vektor
{
namespace creator
{

struct Global
{
    bool        is_break;
    bool        background;

    const char* crashlog_path;
    const char* project_file;
};

extern Global G;

} // namespace creator
} // namespace vektor

#endif
