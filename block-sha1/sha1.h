begin_unit
begin_comment
comment|/*  * Based on the Mozilla SHA1 (see mozilla-sha1/sha1.h),  * optimized to do word accesses rather than byte accesses,  * and to avoid unnecessary copies into the context array.  */
end_comment
begin_typedef
typedef|typedef
struct|struct
block|{
DECL|member|H
name|unsigned
name|int
name|H
index|[
literal|5
index|]
decl_stmt|;
DECL|member|W
name|unsigned
name|int
name|W
index|[
literal|16
index|]
decl_stmt|;
DECL|member|lenW
name|int
name|lenW
decl_stmt|;
DECL|member|size
name|unsigned
name|long
name|long
name|size
decl_stmt|;
block|}
DECL|typedef|blk_SHA_CTX
name|blk_SHA_CTX
typedef|;
end_typedef
begin_function_decl
name|void
name|blk_SHA1_Init
parameter_list|(
name|blk_SHA_CTX
modifier|*
name|ctx
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|blk_SHA1_Update
parameter_list|(
name|blk_SHA_CTX
modifier|*
name|ctx
parameter_list|,
specifier|const
name|void
modifier|*
name|dataIn
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|blk_SHA1_Final
parameter_list|(
name|unsigned
name|char
name|hashout
index|[
literal|20
index|]
parameter_list|,
name|blk_SHA_CTX
modifier|*
name|ctx
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|git_SHA_CTX
define|#
directive|define
name|git_SHA_CTX
value|blk_SHA_CTX
end_define
begin_define
DECL|macro|git_SHA1_Init
define|#
directive|define
name|git_SHA1_Init
value|blk_SHA1_Init
end_define
begin_define
DECL|macro|git_SHA1_Update
define|#
directive|define
name|git_SHA1_Update
value|blk_SHA1_Update
end_define
begin_define
DECL|macro|git_SHA1_Final
define|#
directive|define
name|git_SHA1_Final
value|blk_SHA1_Final
end_define
end_unit
