begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|USERDIFF_H
end_ifndef
begin_define
DECL|macro|USERDIFF_H
define|#
directive|define
name|USERDIFF_H
end_define
begin_include
include|#
directive|include
file|"notes-cache.h"
end_include
begin_struct
DECL|struct|userdiff_funcname
struct|struct
name|userdiff_funcname
block|{
DECL|member|pattern
specifier|const
name|char
modifier|*
name|pattern
decl_stmt|;
DECL|member|cflags
name|int
name|cflags
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|userdiff_driver
struct|struct
name|userdiff_driver
block|{
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|external
specifier|const
name|char
modifier|*
name|external
decl_stmt|;
DECL|member|binary
name|int
name|binary
decl_stmt|;
DECL|member|funcname
name|struct
name|userdiff_funcname
name|funcname
decl_stmt|;
DECL|member|word_regex
specifier|const
name|char
modifier|*
name|word_regex
decl_stmt|;
DECL|member|textconv
specifier|const
name|char
modifier|*
name|textconv
decl_stmt|;
DECL|member|textconv_cache
name|struct
name|notes_cache
modifier|*
name|textconv_cache
decl_stmt|;
DECL|member|textconv_want_cache
name|int
name|textconv_want_cache
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
name|int
name|userdiff_config
parameter_list|(
specifier|const
name|char
modifier|*
name|k
parameter_list|,
specifier|const
name|char
modifier|*
name|v
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|userdiff_driver
modifier|*
name|userdiff_find_by_name
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|userdiff_driver
modifier|*
name|userdiff_find_by_path
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* USERDIFF */
end_comment
end_unit
