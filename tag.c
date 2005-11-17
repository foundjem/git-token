begin_unit
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|tag_type
specifier|const
name|char
modifier|*
name|tag_type
init|=
literal|"tag"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|deref_tag
name|struct
name|object
modifier|*
name|deref_tag
parameter_list|(
name|struct
name|object
modifier|*
name|o
parameter_list|,
specifier|const
name|char
modifier|*
name|warn
parameter_list|,
name|int
name|warnlen
parameter_list|)
block|{
while|while
condition|(
name|o
operator|&&
name|o
operator|->
name|type
operator|==
name|tag_type
condition|)
name|o
operator|=
name|parse_object
argument_list|(
operator|(
operator|(
expr|struct
name|tag
operator|*
operator|)
name|o
operator|)
operator|->
name|tagged
operator|->
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|o
operator|&&
name|warn
condition|)
block|{
if|if
condition|(
operator|!
name|warnlen
condition|)
name|warnlen
operator|=
name|strlen
argument_list|(
name|warn
argument_list|)
expr_stmt|;
name|error
argument_list|(
literal|"missing object referenced by '%.*s'"
argument_list|,
name|warnlen
argument_list|,
name|warn
argument_list|)
expr_stmt|;
block|}
return|return
name|o
return|;
block|}
end_function
begin_function
DECL|function|lookup_tag
name|struct
name|tag
modifier|*
name|lookup_tag
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|lookup_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
block|{
name|struct
name|tag
modifier|*
name|ret
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|tag
argument_list|)
argument_list|)
decl_stmt|;
name|memset
argument_list|(
name|ret
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|tag
argument_list|)
argument_list|)
expr_stmt|;
name|created_object
argument_list|(
name|sha1
argument_list|,
operator|&
name|ret
operator|->
name|object
argument_list|)
expr_stmt|;
name|ret
operator|->
name|object
operator|.
name|type
operator|=
name|tag_type
expr_stmt|;
return|return
name|ret
return|;
block|}
if|if
condition|(
operator|!
name|obj
operator|->
name|type
condition|)
name|obj
operator|->
name|type
operator|=
name|tag_type
expr_stmt|;
if|if
condition|(
name|obj
operator|->
name|type
operator|!=
name|tag_type
condition|)
block|{
name|error
argument_list|(
literal|"Object %s is a %s, not a tree"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|obj
operator|->
name|type
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
return|return
operator|(
expr|struct
name|tag
operator|*
operator|)
name|obj
return|;
block|}
end_function
begin_function
DECL|function|parse_tag_buffer
name|int
name|parse_tag_buffer
parameter_list|(
name|struct
name|tag
modifier|*
name|item
parameter_list|,
name|void
modifier|*
name|data
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|int
name|typelen
decl_stmt|,
name|taglen
decl_stmt|;
name|unsigned
name|char
name|object
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|type_line
decl_stmt|,
modifier|*
name|tag_line
decl_stmt|,
modifier|*
name|sig_line
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|item
operator|->
name|object
operator|.
name|parsed
condition|)
return|return
literal|0
return|;
name|item
operator|->
name|object
operator|.
name|parsed
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|size
operator|<
literal|64
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|memcmp
argument_list|(
literal|"object "
argument_list|,
name|data
argument_list|,
literal|7
argument_list|)
operator|||
name|get_sha1_hex
argument_list|(
name|data
operator|+
literal|7
argument_list|,
name|object
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|type_line
operator|=
name|data
operator|+
literal|48
expr_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
literal|"\ntype "
argument_list|,
name|type_line
operator|-
literal|1
argument_list|,
literal|6
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|tag_line
operator|=
name|strchr
argument_list|(
name|type_line
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tag_line
operator|||
name|memcmp
argument_list|(
literal|"tag "
argument_list|,
operator|++
name|tag_line
argument_list|,
literal|4
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|sig_line
operator|=
name|strchr
argument_list|(
name|tag_line
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|sig_line
condition|)
return|return
operator|-
literal|1
return|;
name|sig_line
operator|++
expr_stmt|;
name|typelen
operator|=
name|tag_line
operator|-
name|type_line
operator|-
name|strlen
argument_list|(
literal|"type \n"
argument_list|)
expr_stmt|;
if|if
condition|(
name|typelen
operator|>=
literal|20
condition|)
return|return
operator|-
literal|1
return|;
name|memcpy
argument_list|(
name|type
argument_list|,
name|type_line
operator|+
literal|5
argument_list|,
name|typelen
argument_list|)
expr_stmt|;
name|type
index|[
name|typelen
index|]
operator|=
literal|'\0'
expr_stmt|;
name|taglen
operator|=
name|sig_line
operator|-
name|tag_line
operator|-
name|strlen
argument_list|(
literal|"tag \n"
argument_list|)
expr_stmt|;
name|item
operator|->
name|tag
operator|=
name|xmalloc
argument_list|(
name|taglen
operator|+
literal|1
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|item
operator|->
name|tag
argument_list|,
name|tag_line
operator|+
literal|4
argument_list|,
name|taglen
argument_list|)
expr_stmt|;
name|item
operator|->
name|tag
index|[
name|taglen
index|]
operator|=
literal|'\0'
expr_stmt|;
name|item
operator|->
name|tagged
operator|=
name|lookup_object_type
argument_list|(
name|object
argument_list|,
name|type
argument_list|)
expr_stmt|;
if|if
condition|(
name|item
operator|->
name|tagged
operator|&&
name|track_object_refs
condition|)
block|{
name|struct
name|object_refs
modifier|*
name|refs
init|=
name|alloc_object_refs
argument_list|(
literal|1
argument_list|)
decl_stmt|;
name|refs
operator|->
name|ref
index|[
literal|0
index|]
operator|=
name|item
operator|->
name|tagged
expr_stmt|;
name|set_object_refs
argument_list|(
operator|&
name|item
operator|->
name|object
argument_list|,
name|refs
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_tag
name|int
name|parse_tag
parameter_list|(
name|struct
name|tag
modifier|*
name|item
parameter_list|)
block|{
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|void
modifier|*
name|data
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|int
name|ret
decl_stmt|;
if|if
condition|(
name|item
operator|->
name|object
operator|.
name|parsed
condition|)
return|return
literal|0
return|;
name|data
operator|=
name|read_sha1_file
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|data
condition|)
return|return
name|error
argument_list|(
literal|"Could not read %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|strcmp
argument_list|(
name|type
argument_list|,
name|tag_type
argument_list|)
condition|)
block|{
name|free
argument_list|(
name|data
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"Object %s not a tag"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
return|;
block|}
name|ret
operator|=
name|parse_tag_buffer
argument_list|(
name|item
argument_list|,
name|data
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|data
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
end_unit
