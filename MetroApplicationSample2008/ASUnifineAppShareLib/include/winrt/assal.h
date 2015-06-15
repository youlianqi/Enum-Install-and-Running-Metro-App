#pragma once

#ifndef _Group_
#define _Group_(a)
#endif

#ifndef _SA_annotes3
#define _SA_annotes3(n,pp1,pp2,pp3)
#endif

#ifndef _Out_range_impl_
#define _Out_range_impl_(min,max)
#endif

#ifndef _SAL2_Source_
#define _SAL2_Source_(Name, args, annotes) _SA_annotes3(SAL_name, #Name, "", "2") _Group_(annotes _SAL_nop_impl_)
#endif

#ifndef _SAL1_2_Source_
#define _SAL1_2_Source_(Name, args, annotes) _SA_annotes3(SAL_name, #Name, "", "1.2") _Group_(annotes _SAL_nop_impl_)
#endif

#ifndef _Out_range_
#define _Out_range_(lb,ub)          _SAL2_Source_(_Out_range_, (lb,ub), _Out_range_impl_(lb,ub))
#endif 

#ifndef _Post_equal_to_
#define _Post_equal_to_(expr)       _SAL2_Source_(_Post_equal_to_, (expr), _Out_range_(==, expr))
#endif

#ifndef _Post_satisfies_impl_
#define _Post_satisfies_impl_(expr)
#endif

#ifndef _Post_satisfies_
#define _Post_satisfies_(cond)      _SAL2_Source_(_Post_satisfies_, (cond), _Post_satisfies_impl_(cond))
#endif

#ifndef __RPC__inout_xcount
#define __RPC__inout_xcount(size)                    _SAL1_2_Source_(__RPC__inout_xcount, (size), _Inout_)
#endif

#ifndef __RPC__in_xcount
#define __RPC__in_xcount(size)                       _SAL1_2_Source_(__RPC__in_xcount, (size), __RPC__in _Pre_readable_size_(size))
#endif

#ifndef _Null_terminated_impl_
#define _Null_terminated_impl_
#endif

#ifndef _Null_terminated_
#define _Null_terminated_                 _SAL2_Source_(_Null_terminated_, (), _Null_terminated_impl_)
#endif

#ifndef WINAPI_FAMILY_PARTITION
#define WINAPI_FAMILY_PARTITION(Partitions)     (Partitions)
#endif

#ifndef _Outptr_result_maybenull_
#define _Outptr_result_maybenull_                      __allowed(on_parameter)
#endif

#ifndef _Inout_updates_
#define _Inout_updates_(size)                  _SAL2_Source_(_Inout_updates_, (size), _Pre_cap_(size)         _Pre_valid_impl_ _Post_valid_impl_)
#endif
 
#ifndef _Out_writes_to_
#define _Out_writes_to_(size,count)            _SAL2_Source_(_Out_writes_to_, (size,count), _Pre_cap_(size)            _Post_valid_impl_ _Post_count_(count))
#endif

#ifndef _In_reads_
#define _In_reads_(size)               _SAL2_Source_(_In_reads_, (size), _Pre_count_(size)          _Deref_pre_readonly_)
#endif


#ifndef _Outptr_result_buffer_
#define _Outptr_result_buffer_(size)                   __allowed(on_parameter)
#endif


#ifdef __cplusplus

extern "C++" {

	template <size_t S>
	struct _ENUM_FLAG_INTEGER_FOR_SIZE;

	template <>
	struct _ENUM_FLAG_INTEGER_FOR_SIZE<1>
	{
		typedef INT8 type;
	};

	template <>
	struct _ENUM_FLAG_INTEGER_FOR_SIZE<2>
	{
		typedef INT16 type;
	};

	template <>
	struct _ENUM_FLAG_INTEGER_FOR_SIZE<4>
	{
		typedef INT32 type;
	};

	// used as an approximation of std::underlying_type<T>
	template <class T>
	struct _ENUM_FLAG_SIZED_INTEGER
	{
		typedef typename _ENUM_FLAG_INTEGER_FOR_SIZE<sizeof(T)>::type type;
	};

}

#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
	extern "C++" { \
	inline ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) | ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
	inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) |= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
	inline ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) & ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
	inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) &= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
	inline ENUMTYPE operator ~ (ENUMTYPE a) { return ENUMTYPE(~((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)); } \
	inline ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) ^ ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
	inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) ^= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
}
#else
#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) // NOP, C allows these operators.
#endif

#ifndef __RPC__in_range
#define __RPC__in_range(min,max)                         _SAL1_2_Source_(__RPC__in_range, (min,max), _In_range_(min,max))
#endif

#ifndef E_BOUNDS
#define E_BOUNDS                         _HRESULT_TYPEDEF_(0x8000000BL)
#endif

#ifndef _Check_return_
#define _Check_return_           _SAL2_Source_(_Check_return_, (), _Check_return_impl_)
#endif

#ifndef _COM_Outptr_
#define _COM_Outptr_                                   __allowed(on_parameter)
#endif

#ifndef _Out_writes_bytes_
#define _Out_writes_bytes_(size)               _SAL2_Source_(_Out_writes_bytes_, (size), _Pre_bytecap_(size)        _Post_valid_impl_)
#endif

#ifndef _When_
#define _When_(expr, annotes)    __allowed(on_parameter_or_return)
#endif

#ifndef _In_reads_opt_
#define _In_reads_opt_(size)           _SAL2_Source_(_In_reads_opt_, (size), _Pre_opt_count_(size)      _Deref_pre_readonly_)
#endif

#ifndef _Out_opt_
#define _Out_opt_                              _SAL2_Source_(_Out_opt_, (), _Out_opt_impl_)
#endif

#ifndef _Outptr_
#define _Outptr_                                       __allowed(on_parameter)
#endif

#ifndef _Outptr_result_nullonfailure_
#define _Outptr_result_nullonfailure_                  __allowed(on_parameter)
#endif

#ifndef _Reserved_
#define _Reserved_                      _SAL2_Source_(_Reserved_, (), _Pre1_impl_(__null_impl))
#endif

#ifndef _Out_writes_bytes_to_
#define _Out_writes_bytes_to_(size,count)      _SAL2_Source_(_Out_writes_bytes_to_, (size,count), _Pre_bytecap_(size)        _Post_valid_impl_ _Post_bytecount_(count))
#endif

#ifndef _In_reads_bytes_
#define _In_reads_bytes_(size)         _SAL2_Source_(_In_reads_bytes_, (size), _Pre_bytecount_(size)      _Deref_pre_readonly_)
#endif

#ifndef _Outptr_opt_
#define _Outptr_opt_                                   __allowed(on_parameter)
#endif

#ifndef _Out_writes_opt_
#define _Out_writes_opt_(size)                 _SAL2_Source_(_Out_writes_opt_, (size), _Pre_opt_cap_(size)        _Post_valid_impl_)
#endif
