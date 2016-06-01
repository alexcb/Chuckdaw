#pragma once
#include <uuid/uuid.h>

#define uuid_to_string(UUID_S, UUID)  char UUID_S[37]; uuid_unparse(UUID, UUID_S);
