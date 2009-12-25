begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|RESOLVE_UNDO_H
end_ifndef
begin_define
DECL|macro|RESOLVE_UNDO_H
define|#
directive|define
name|RESOLVE_UNDO_H
end_define
begin_struct
DECL|struct|resolve_undo_info
struct|struct
name|resolve_undo_info
block|{
DECL|member|mode
name|unsigned
name|int
name|mode
index|[
literal|3
index|]
decl_stmt|;
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|3
index|]
index|[
literal|20
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|void
name|record_resolve_undo
parameter_list|(
name|struct
name|index_state
modifier|*
parameter_list|,
name|struct
name|cache_entry
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|resolve_undo_write
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|struct
name|string_list
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|string_list
modifier|*
name|resolve_undo_read
parameter_list|(
name|void
modifier|*
parameter_list|,
name|unsigned
name|long
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|resolve_undo_clear_index
parameter_list|(
name|struct
name|index_state
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
