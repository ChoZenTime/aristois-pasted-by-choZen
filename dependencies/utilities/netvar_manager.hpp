#pragma once
#include <windows.h>
#include <SDKDDKVer.h>
#include <string.h>
#include <vadefs.h>
#include <stdio.h>
#include <xstring>
#include <Psapi.h>
#include <thread>
#include <iostream>
#include <algorithm>
#include <functional>
#include <numeric>
#include <string>
#include <vector>
#include <time.h>
#include <winuser.h>
#include <random>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <set>
#include <map>
#include <unordered_map>
#include <memory>
#include <thread>
#include <array>
#include <assert.h>
#include <deque>
#include <intrin.h>
#include <atomic>
#include <mutex>
#include <d3dx9.h>
#include <d3d9.h>
#include <stdint.h>
#include <stdlib.h>
#include <process.h>
#include <Wincon.h>
#include <cstdint>
#include <chrono>
#include <Shlobj.h>
#include <future>
#include <Lmcons.h>
#include <tchar.h>
#include "../../source-sdk/classes/recv_props.hpp"
#include "../../source-sdk/classes/client_class.hpp"

namespace netvar_manager {
#pragma warning( push )
#pragma warning( disable: 4307 )
	namespace fnv_1a {
		template< typename S >
		struct fnv_internal;
		template< typename S >
		struct fnv1a;

		template< >
		struct fnv_internal< uint32_t > {
			constexpr static uint32_t default_offset_basis = 0x811C9DC5;
			constexpr static uint32_t prime = 0x01000193;
		};

		template< >
		struct fnv1a< uint32_t > : public fnv_internal< uint32_t > {
			constexpr static uint32_t hash(char const* const aString, const uint32_t val = default_offset_basis) {
				return (aString[0] == '\0')
					? val
					: hash(&aString[1], (val ^ uint32_t(aString[0])) * prime);
			}

			constexpr static uint32_t hash(wchar_t const* const aString, const uint32_t val = default_offset_basis) {
				return (aString[0] == L'\0')
					? val
					: hash(&aString[1], (val ^ uint32_t(aString[0])) * prime);
			}
		};
	}

	using fnv = fnv_1a::fnv1a< uint32_t >;

#pragma warning( pop )
}

#define NETVAR(table, prop, func_name, type) \
	type& func_name( ) { \
      static uintptr_t offset = 0; \
      if(!offset) \
      { offset = netvar_manager::get_net_var(netvar_manager::fnv::hash( table ), netvar_manager::fnv::hash( prop ) ); } \
	  \
      return *reinterpret_cast< type* >( uintptr_t( this ) + offset ); \
    }

#define NETVAR_PTR(table, prop, func_name, type) \
	type* func_name( ) { \
      static uintptr_t offset = 0; \
      if(!offset) \
      { offset = netvar_manager::get_net_var(netvar_manager::fnv::hash( table ), netvar_manager::fnv::hash( prop ) ); } \
	  \
      return reinterpret_cast< type* >( uintptr_t( this ) + offset ); \
    }

#define OFFSET(type, var, offset) \
	type& var() { \
		return *(type*)(uintptr_t(this) + offset); \
	} \

namespace netvar_manager {
	uintptr_t get_net_var(uint32_t table, uint32_t prop);
}