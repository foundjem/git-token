begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_decl_stmt
DECL|variable|is_shallow
specifier|static
name|int
name|is_shallow
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|register_shallow
name|int
name|register_shallow
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|commit_graft
modifier|*
name|graft
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|commit_graft
argument_list|)
argument_list|)
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
init|=
name|lookup_commit
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|hashcpy
argument_list|(
name|graft
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
name|graft
operator|->
name|nr_parent
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|commit
operator|&&
name|commit
operator|->
name|object
operator|.
name|parsed
condition|)
name|commit
operator|->
name|parents
operator|=
name|NULL
expr_stmt|;
return|return
name|register_commit_graft
argument_list|(
name|graft
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|is_repository_shallow
name|int
name|is_repository_shallow
parameter_list|(
name|void
parameter_list|)
block|{
name|FILE
modifier|*
name|fp
decl_stmt|;
name|char
name|buf
index|[
literal|1024
index|]
decl_stmt|;
if|if
condition|(
name|is_shallow
operator|>=
literal|0
condition|)
return|return
name|is_shallow
return|;
name|fp
operator|=
name|fopen
argument_list|(
name|git_path
argument_list|(
literal|"shallow"
argument_list|)
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|fp
condition|)
block|{
name|is_shallow
operator|=
literal|0
expr_stmt|;
return|return
name|is_shallow
return|;
block|}
name|is_shallow
operator|=
literal|1
expr_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|,
name|fp
argument_list|)
condition|)
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
name|get_sha1_hex
argument_list|(
name|buf
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"bad shallow line: %s"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
name|register_shallow
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
block|}
name|fclose
argument_list|(
name|fp
argument_list|)
expr_stmt|;
return|return
name|is_shallow
return|;
block|}
end_function
begin_function
DECL|function|get_shallow_commits
name|struct
name|commit_list
modifier|*
name|get_shallow_commits
parameter_list|(
name|struct
name|object_array
modifier|*
name|heads
parameter_list|,
name|int
name|depth
parameter_list|,
name|int
name|shallow_flag
parameter_list|,
name|int
name|not_shallow_flag
parameter_list|)
block|{
name|int
name|i
init|=
literal|0
decl_stmt|,
name|cur_depth
init|=
literal|0
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|result
init|=
name|NULL
decl_stmt|;
name|struct
name|object_array
name|stack
init|=
name|OBJECT_ARRAY_INIT
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
init|=
name|NULL
decl_stmt|;
while|while
condition|(
name|commit
operator|||
name|i
operator|<
name|heads
operator|->
name|nr
operator|||
name|stack
operator|.
name|nr
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
block|{
if|if
condition|(
name|i
operator|<
name|heads
operator|->
name|nr
condition|)
block|{
name|commit
operator|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|deref_tag
argument_list|(
name|heads
operator|->
name|objects
index|[
name|i
operator|++
index|]
operator|.
name|item
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
operator|||
name|commit
operator|->
name|object
operator|.
name|type
operator|!=
name|OBJ_COMMIT
condition|)
block|{
name|commit
operator|=
name|NULL
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|commit
operator|->
name|util
condition|)
name|commit
operator|->
name|util
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|int
argument_list|)
argument_list|)
expr_stmt|;
operator|*
operator|(
name|int
operator|*
operator|)
name|commit
operator|->
name|util
operator|=
literal|0
expr_stmt|;
name|cur_depth
operator|=
literal|0
expr_stmt|;
block|}
else|else
block|{
name|commit
operator|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|stack
operator|.
name|objects
index|[
operator|--
name|stack
operator|.
name|nr
index|]
operator|.
name|item
expr_stmt|;
name|cur_depth
operator|=
operator|*
operator|(
name|int
operator|*
operator|)
name|commit
operator|->
name|util
expr_stmt|;
block|}
block|}
if|if
condition|(
name|parse_commit
argument_list|(
name|commit
argument_list|)
condition|)
name|die
argument_list|(
literal|"invalid commit"
argument_list|)
expr_stmt|;
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|not_shallow_flag
expr_stmt|;
name|cur_depth
operator|++
expr_stmt|;
for|for
control|(
name|p
operator|=
name|commit
operator|->
name|parents
operator|,
name|commit
operator|=
name|NULL
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
if|if
condition|(
operator|!
name|p
operator|->
name|item
operator|->
name|util
condition|)
block|{
name|int
modifier|*
name|pointer
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|int
argument_list|)
argument_list|)
decl_stmt|;
name|p
operator|->
name|item
operator|->
name|util
operator|=
name|pointer
expr_stmt|;
operator|*
name|pointer
operator|=
name|cur_depth
expr_stmt|;
block|}
else|else
block|{
name|int
modifier|*
name|pointer
init|=
name|p
operator|->
name|item
operator|->
name|util
decl_stmt|;
if|if
condition|(
name|cur_depth
operator|>=
operator|*
name|pointer
condition|)
continue|continue;
operator|*
name|pointer
operator|=
name|cur_depth
expr_stmt|;
block|}
if|if
condition|(
name|cur_depth
operator|<
name|depth
condition|)
block|{
if|if
condition|(
name|p
operator|->
name|next
condition|)
name|add_object_array
argument_list|(
operator|&
name|p
operator|->
name|item
operator|->
name|object
argument_list|,
name|NULL
argument_list|,
operator|&
name|stack
argument_list|)
expr_stmt|;
else|else
block|{
name|commit
operator|=
name|p
operator|->
name|item
expr_stmt|;
name|cur_depth
operator|=
operator|*
operator|(
name|int
operator|*
operator|)
name|commit
operator|->
name|util
expr_stmt|;
block|}
block|}
else|else
block|{
name|commit_list_insert
argument_list|(
name|p
operator|->
name|item
argument_list|,
operator|&
name|result
argument_list|)
expr_stmt|;
name|p
operator|->
name|item
operator|->
name|object
operator|.
name|flags
operator||=
name|shallow_flag
expr_stmt|;
block|}
block|}
block|}
return|return
name|result
return|;
block|}
end_function
end_unit
