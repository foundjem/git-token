begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|hexval_table
specifier|const
name|signed
name|char
name|hexval_table
index|[
literal|256
index|]
init|=
block|{
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 00-07 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 08-0f */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 10-17 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 18-1f */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 20-27 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 28-2f */
literal|0
block|,
literal|1
block|,
literal|2
block|,
literal|3
block|,
literal|4
block|,
literal|5
block|,
literal|6
block|,
literal|7
block|,
comment|/* 30-37 */
literal|8
block|,
literal|9
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 38-3f */
operator|-
literal|1
block|,
literal|10
block|,
literal|11
block|,
literal|12
block|,
literal|13
block|,
literal|14
block|,
literal|15
block|,
operator|-
literal|1
block|,
comment|/* 40-47 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 48-4f */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 50-57 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 58-5f */
operator|-
literal|1
block|,
literal|10
block|,
literal|11
block|,
literal|12
block|,
literal|13
block|,
literal|14
block|,
literal|15
block|,
operator|-
literal|1
block|,
comment|/* 60-67 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 68-67 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 70-77 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 78-7f */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 80-87 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 88-8f */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 90-97 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* 98-9f */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* a0-a7 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* a8-af */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* b0-b7 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* b8-bf */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* c0-c7 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* c8-cf */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* d0-d7 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* d8-df */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* e0-e7 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* e8-ef */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* f0-f7 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
operator|-
literal|1
block|,
comment|/* f8-ff */
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|get_sha1_hex
name|int
name|get_sha1_hex
parameter_list|(
specifier|const
name|char
modifier|*
name|hex
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|GIT_SHA1_RAWSZ
condition|;
name|i
operator|++
control|)
block|{
name|unsigned
name|int
name|val
decl_stmt|;
comment|/* 		 * hex[1]=='\0' is caught when val is checked below, 		 * but if hex[0] is NUL we have to avoid reading 		 * past the end of the string: 		 */
if|if
condition|(
operator|!
name|hex
index|[
literal|0
index|]
condition|)
return|return
operator|-
literal|1
return|;
name|val
operator|=
operator|(
name|hexval
argument_list|(
name|hex
index|[
literal|0
index|]
argument_list|)
operator|<<
literal|4
operator|)
operator||
name|hexval
argument_list|(
name|hex
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|val
operator|&
operator|~
literal|0xff
condition|)
return|return
operator|-
literal|1
return|;
operator|*
name|sha1
operator|++
operator|=
name|val
expr_stmt|;
name|hex
operator|+=
literal|2
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_oid_hex
name|int
name|get_oid_hex
parameter_list|(
specifier|const
name|char
modifier|*
name|hex
parameter_list|,
name|struct
name|object_id
modifier|*
name|oid
parameter_list|)
block|{
return|return
name|get_sha1_hex
argument_list|(
name|hex
argument_list|,
name|oid
operator|->
name|hash
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|sha1_to_hex_r
name|char
modifier|*
name|sha1_to_hex_r
parameter_list|(
name|char
modifier|*
name|buffer
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
specifier|const
name|char
name|hex
index|[]
init|=
literal|"0123456789abcdef"
decl_stmt|;
name|char
modifier|*
name|buf
init|=
name|buffer
decl_stmt|;
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|GIT_SHA1_RAWSZ
condition|;
name|i
operator|++
control|)
block|{
name|unsigned
name|int
name|val
init|=
operator|*
name|sha1
operator|++
decl_stmt|;
operator|*
name|buf
operator|++
operator|=
name|hex
index|[
name|val
operator|>>
literal|4
index|]
expr_stmt|;
operator|*
name|buf
operator|++
operator|=
name|hex
index|[
name|val
operator|&
literal|0xf
index|]
expr_stmt|;
block|}
operator|*
name|buf
operator|=
literal|'\0'
expr_stmt|;
return|return
name|buffer
return|;
block|}
end_function
begin_function
DECL|function|sha1_to_hex
name|char
modifier|*
name|sha1_to_hex
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
name|int
name|bufno
decl_stmt|;
specifier|static
name|char
name|hexbuffer
index|[
literal|4
index|]
index|[
name|GIT_SHA1_HEXSZ
operator|+
literal|1
index|]
decl_stmt|;
return|return
name|sha1_to_hex_r
argument_list|(
name|hexbuffer
index|[
literal|3
operator|&
operator|++
name|bufno
index|]
argument_list|,
name|sha1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|oid_to_hex
name|char
modifier|*
name|oid_to_hex
parameter_list|(
specifier|const
name|struct
name|object_id
modifier|*
name|oid
parameter_list|)
block|{
return|return
name|sha1_to_hex
argument_list|(
name|oid
operator|->
name|hash
argument_list|)
return|;
block|}
end_function
end_unit
