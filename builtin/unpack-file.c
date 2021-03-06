begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_function
DECL|function|create_temp_file
specifier|static
name|char
modifier|*
name|create_temp_file
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
name|char
name|path
index|[
literal|50
index|]
decl_stmt|;
name|void
modifier|*
name|buf
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|buf
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
operator|||
name|type
operator|!=
name|OBJ_BLOB
condition|)
name|die
argument_list|(
literal|"unable to read blob object %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|xsnprintf
argument_list|(
name|path
argument_list|,
sizeof|sizeof
argument_list|(
name|path
argument_list|)
argument_list|,
literal|".merge_file_XXXXXX"
argument_list|)
expr_stmt|;
name|fd
operator|=
name|xmkstemp
argument_list|(
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_in_full
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
operator|!=
name|size
condition|)
name|die_errno
argument_list|(
literal|"unable to write temp-file"
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
return|return
name|path
return|;
block|}
end_function
begin_function
DECL|function|cmd_unpack_file
name|int
name|cmd_unpack_file
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|argc
operator|!=
literal|2
operator|||
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-h"
argument_list|)
condition|)
name|usage
argument_list|(
literal|"git unpack-file<sha1>"
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Not a valid object name %s"
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|puts
argument_list|(
name|create_temp_file
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
