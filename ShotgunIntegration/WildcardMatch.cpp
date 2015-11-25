// Copyright (c) 2013 Shotgun Software Inc.

#include <string>
#include <ctype.h>

#include "WildcardMatch.h"

bool WildcardMatch(const std::string &pattern, const std::string &str) {
   int i, star;
   const char *c_pat = pattern.c_str();
   const char *c_str = str.c_str();
    
new_segment:

   star = 0;
   if (*c_pat == '*') {
      star = 1;
      do { c_pat++; }
      while (*c_pat == '*');
   } /* endif */

test_match:

   for (i = 0; c_pat[i] && (c_pat[i] != '*'); i++) {
      if (toupper(c_str[i]) != toupper(c_pat[i])) {
         if (!c_str[i])
            return false;
         if ((c_pat[i] == '?') && (c_str[i] != '.'))
            continue;
         if (!star)
            return false;
         c_str++;
         goto test_match;
      }
   }
   if (c_pat[i] == '*') {
      c_str += i;
      c_pat += i;
      goto new_segment;
   }
   if (!c_str[i])
      return true;
   if (i && c_pat[i - 1] == '*')
      return true;
   if (!star)
      return false;
   c_str++;
   goto test_match;
}
