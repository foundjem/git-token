begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|SUBMODULE_H
end_ifndef
begin_define
DECL|macro|SUBMODULE_H
define|#
directive|define
name|SUBMODULE_H
end_define
begin_function_decl
name|void
name|show_submodule_summary
parameter_list|(
name|FILE
modifier|*
name|f
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|char
name|one
index|[
literal|20
index|]
parameter_list|,
name|unsigned
name|char
name|two
index|[
literal|20
index|]
parameter_list|,
specifier|const
name|char
modifier|*
name|del
parameter_list|,
specifier|const
name|char
modifier|*
name|add
parameter_list|,
specifier|const
name|char
modifier|*
name|reset
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
