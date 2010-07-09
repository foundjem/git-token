begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|THREAD_COMPAT_H
end_ifndef
begin_define
DECL|macro|THREAD_COMPAT_H
define|#
directive|define
name|THREAD_COMPAT_H
end_define
begin_function_decl
specifier|extern
name|int
name|online_cpus
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|init_recursive_mutex
parameter_list|(
name|pthread_mutex_t
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* THREAD_COMPAT_H */
end_comment
end_unit
