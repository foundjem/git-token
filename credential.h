begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|CREDENTIAL_H
end_ifndef
begin_define
DECL|macro|CREDENTIAL_H
define|#
directive|define
name|CREDENTIAL_H
end_define
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_struct
DECL|struct|credential
struct|struct
name|credential
block|{
DECL|member|helpers
name|struct
name|string_list
name|helpers
decl_stmt|;
DECL|member|approved
name|unsigned
name|approved
range|:
literal|1
decl_stmt|,
DECL|member|configured
name|configured
range|:
literal|1
decl_stmt|,
DECL|member|quit
name|quit
range|:
literal|1
decl_stmt|,
DECL|member|use_http_path
name|use_http_path
range|:
literal|1
decl_stmt|;
DECL|member|username
name|char
modifier|*
name|username
decl_stmt|;
DECL|member|password
name|char
modifier|*
name|password
decl_stmt|;
DECL|member|protocol
name|char
modifier|*
name|protocol
decl_stmt|;
DECL|member|host
name|char
modifier|*
name|host
decl_stmt|;
DECL|member|path
name|char
modifier|*
name|path
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|CREDENTIAL_INIT
define|#
directive|define
name|CREDENTIAL_INIT
value|{ STRING_LIST_INIT_DUP }
end_define
begin_function_decl
name|void
name|credential_init
parameter_list|(
name|struct
name|credential
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|credential_clear
parameter_list|(
name|struct
name|credential
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|credential_fill
parameter_list|(
name|struct
name|credential
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|credential_approve
parameter_list|(
name|struct
name|credential
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|credential_reject
parameter_list|(
name|struct
name|credential
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|credential_read
parameter_list|(
name|struct
name|credential
modifier|*
parameter_list|,
name|FILE
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|credential_write
parameter_list|(
specifier|const
name|struct
name|credential
modifier|*
parameter_list|,
name|FILE
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|credential_from_url
parameter_list|(
name|struct
name|credential
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
name|url
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|credential_match
parameter_list|(
specifier|const
name|struct
name|credential
modifier|*
name|have
parameter_list|,
specifier|const
name|struct
name|credential
modifier|*
name|want
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* CREDENTIAL_H */
end_comment
end_unit
