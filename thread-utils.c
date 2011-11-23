begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"thread-utils.h"
end_include
begin_if
if|#
directive|if
name|defined
argument_list|(
name|hpux
argument_list|)
operator|||
name|defined
argument_list|(
name|__hpux
argument_list|)
operator|||
name|defined
argument_list|(
name|_hpux
argument_list|)
end_if
begin_include
include|#
directive|include
file|<sys/pstat.h>
end_include
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/*  * By doing this in two steps we can at least get  * the function to be somewhat coherent, even  * with this disgusting nest of #ifdefs.  */
end_comment
begin_ifndef
ifndef|#
directive|ifndef
name|_SC_NPROCESSORS_ONLN
end_ifndef
begin_ifdef
ifdef|#
directive|ifdef
name|_SC_NPROC_ONLN
end_ifdef
begin_define
DECL|macro|_SC_NPROCESSORS_ONLN
define|#
directive|define
name|_SC_NPROCESSORS_ONLN
value|_SC_NPROC_ONLN
end_define
begin_elif
elif|#
directive|elif
name|defined
name|_SC_CRAY_NCPU
end_elif
begin_define
DECL|macro|_SC_NPROCESSORS_ONLN
define|#
directive|define
name|_SC_NPROCESSORS_ONLN
value|_SC_CRAY_NCPU
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|online_cpus
name|int
name|online_cpus
parameter_list|(
name|void
parameter_list|)
block|{
ifdef|#
directive|ifdef
name|_SC_NPROCESSORS_ONLN
name|long
name|ncpus
decl_stmt|;
endif|#
directive|endif
ifdef|#
directive|ifdef
name|_WIN32
name|SYSTEM_INFO
name|info
decl_stmt|;
name|GetSystemInfo
argument_list|(
operator|&
name|info
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|int
operator|)
name|info
operator|.
name|dwNumberOfProcessors
operator|>
literal|0
condition|)
return|return
operator|(
name|int
operator|)
name|info
operator|.
name|dwNumberOfProcessors
return|;
elif|#
directive|elif
name|defined
argument_list|(
name|hpux
argument_list|)
operator|||
name|defined
argument_list|(
name|__hpux
argument_list|)
operator|||
name|defined
argument_list|(
name|_hpux
argument_list|)
name|struct
name|pst_dynamic
name|psd
decl_stmt|;
if|if
condition|(
operator|!
name|pstat_getdynamic
argument_list|(
operator|&
name|psd
argument_list|,
sizeof|sizeof
argument_list|(
name|psd
argument_list|)
argument_list|,
operator|(
name|size_t
operator|)
literal|1
argument_list|,
literal|0
argument_list|)
condition|)
return|return
operator|(
name|int
operator|)
name|psd
operator|.
name|psd_proc_cnt
return|;
endif|#
directive|endif
ifdef|#
directive|ifdef
name|_SC_NPROCESSORS_ONLN
if|if
condition|(
operator|(
name|ncpus
operator|=
operator|(
name|long
operator|)
name|sysconf
argument_list|(
name|_SC_NPROCESSORS_ONLN
argument_list|)
operator|)
operator|>
literal|0
condition|)
return|return
operator|(
name|int
operator|)
name|ncpus
return|;
endif|#
directive|endif
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|init_recursive_mutex
name|int
name|init_recursive_mutex
parameter_list|(
name|pthread_mutex_t
modifier|*
name|m
parameter_list|)
block|{
name|pthread_mutexattr_t
name|a
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|ret
operator|=
name|pthread_mutexattr_init
argument_list|(
operator|&
name|a
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
block|{
name|ret
operator|=
name|pthread_mutexattr_settype
argument_list|(
operator|&
name|a
argument_list|,
name|PTHREAD_MUTEX_RECURSIVE
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|ret
operator|=
name|pthread_mutex_init
argument_list|(
name|m
argument_list|,
operator|&
name|a
argument_list|)
expr_stmt|;
name|pthread_mutexattr_destroy
argument_list|(
operator|&
name|a
argument_list|)
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
end_unit
