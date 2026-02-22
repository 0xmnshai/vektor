// #pragma once 

// namespace vektor
// {
// void view2d_zoom_cache_reset()
// {
//   /* TODO(sergey): This way we avoid threading conflict with sequencer rendering
//    * text strip. But ideally we want to make glyph cache to be fully safe
//    * for threading.
//    */
//   if (G.is_rendering) {
//     return;
//   }
//   /* While scaling we can accumulate fonts at many sizes (~20 or so).
//    * Not an issue with embedded font, but can use over 500Mb with i18n ones! See #38244. */

//   /* NOTE: only some views draw text, we could check for this case to avoid cleaning cache. */
//   BLF_cache_clear();
// }
// }
