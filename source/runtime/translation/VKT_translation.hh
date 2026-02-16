
#pragma once

#include "../vklib/VKE_string_ref.hh"

using namespace vektor::vklib;

namespace vektor
{
#define TEXT_DOMAIN_NAME "vektor"

StringRef  VKT_translate_do_iface(StringRef msgctxt,
                                   StringRef msgid);
const char*VKT_translate_do_tooltip(const char* msgctxt,
                                     const char* msgid);
StringRef  VKT_translate_do_tooltip(StringRef msgctxt,
                                     StringRef msgid);
const char*VKT_translate_do_report(const char* msgctxt,
                                    const char* msgid);
StringRef  VKT_translate_do_report(StringRef msgctxt,
                                    StringRef msgid);
const char*VKT_translate_do_new_dataname(const char* msgctxt,
                                          const char* msgid);
StringRef  VKT_translate_do_new_dataname(StringRef msgctxt,
                                          StringRef msgid);

// For UI in vektor
#define IFACE_(msgid) VKT_translate_do_iface(NULL, msgid)
#define TIP_(msgid) VKT_translate_do_tooltip(NULL, msgid)
#define RPT_(msgid) VKT_translate_do_report(NULL, msgid)
#define DATA_(msgid) VKT_translate_do_new_dataname(NULL, msgid)
#define CTX_IFACE_(context, msgid) VKT_translate_do_iface(context, msgid)
#define CTX_TIP_(context, msgid) VKT_translate_do_tooltip(context, msgid)
#define CTX_RPT_(context, msgid) VKT_translate_do_report(context, msgid)
#define CTX_DATA_(context, msgid) VKT_translate_do_new_dataname(context, msgid)

}; // namespace vektor