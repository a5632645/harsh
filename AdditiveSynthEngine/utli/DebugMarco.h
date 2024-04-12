#ifdef MANA_DEBUG
#define MANA_DEBUG_DEFINE(TYPE,NAME,VALUE) TYPE NAME = VALUE
#else
#define MANA_DEBUG_DEFINE(TYPE,NAME,VALUE)
#endif // MANA_DEBUG

#define DSP_INIT_DEFINE MANA_DEBUG_DEFINE(bool,m_is_init,false)

#ifdef MANA_DEBUG
#define DSP_INIT m_is_init = true
#else
#define DSP_INIT
#endif // MANA_DEBUG

#ifdef MANA_DEBUG
#include <cassert>
#define MANA_ASSERT(X) assert(X)
#else
#define MANA_ASSERT(X)
#endif // MANA_DEBUG

#define DSP_INIT_ASSERT MANA_ASSERT(m_is_init)

#define MANA_UNREACHABLE_CODE MANA_ASSERT(false)