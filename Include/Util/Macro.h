#pragma once

#define NO_COPY_ASSIGN(_Class)                                                                                                     \
   _Class(const _Class&) = delete;                                                                                                 \
   _Class& operator=(const _Class&) = delete

#define UNUSED(_VARIABLE) (void)_VARIABLE
