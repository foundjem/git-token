begin_unit
begin_comment
comment|/*  * decorate.c - decorate a git object with some arbitrary  * data.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_include
include|#
directive|include
file|"decorate.h"
end_include
begin_function
DECL|function|hash_obj
specifier|static
name|unsigned
name|int
name|hash_obj
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
name|unsigned
name|int
name|n
parameter_list|)
block|{
name|unsigned
name|int
name|hash
init|=
operator|*
operator|(
name|unsigned
name|int
operator|*
operator|)
name|obj
operator|->
name|sha1
decl_stmt|;
return|return
name|hash
operator|%
name|n
return|;
block|}
end_function
begin_function
DECL|function|insert_decoration
specifier|static
name|void
modifier|*
name|insert_decoration
parameter_list|(
name|struct
name|decoration
modifier|*
name|n
parameter_list|,
name|struct
name|object
modifier|*
name|base
parameter_list|,
name|void
modifier|*
name|decoration
parameter_list|)
block|{
name|int
name|size
init|=
name|n
operator|->
name|size
decl_stmt|;
name|struct
name|object_decoration
modifier|*
name|hash
init|=
name|n
operator|->
name|hash
decl_stmt|;
name|int
name|j
init|=
name|hash_obj
argument_list|(
name|base
argument_list|,
name|size
argument_list|)
decl_stmt|;
while|while
condition|(
name|hash
index|[
name|j
index|]
operator|.
name|base
condition|)
block|{
if|if
condition|(
name|hash
index|[
name|j
index|]
operator|.
name|base
operator|==
name|base
condition|)
block|{
name|void
modifier|*
name|old
init|=
name|hash
index|[
name|j
index|]
operator|.
name|decoration
decl_stmt|;
name|hash
index|[
name|j
index|]
operator|.
name|decoration
operator|=
name|decoration
expr_stmt|;
return|return
name|old
return|;
block|}
if|if
condition|(
operator|++
name|j
operator|>=
name|size
condition|)
name|j
operator|=
literal|0
expr_stmt|;
block|}
name|hash
index|[
name|j
index|]
operator|.
name|base
operator|=
name|base
expr_stmt|;
name|hash
index|[
name|j
index|]
operator|.
name|decoration
operator|=
name|decoration
expr_stmt|;
name|n
operator|->
name|nr
operator|++
expr_stmt|;
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|grow_decoration
specifier|static
name|void
name|grow_decoration
parameter_list|(
name|struct
name|decoration
modifier|*
name|n
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|old_size
init|=
name|n
operator|->
name|size
decl_stmt|;
name|struct
name|object_decoration
modifier|*
name|old_hash
decl_stmt|;
name|old_size
operator|=
name|n
operator|->
name|size
expr_stmt|;
name|old_hash
operator|=
name|n
operator|->
name|hash
expr_stmt|;
name|n
operator|->
name|size
operator|=
operator|(
name|old_size
operator|+
literal|1000
operator|)
operator|*
literal|3
operator|/
literal|2
expr_stmt|;
name|n
operator|->
name|hash
operator|=
name|xcalloc
argument_list|(
name|n
operator|->
name|size
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|object_decoration
argument_list|)
argument_list|)
expr_stmt|;
name|n
operator|->
name|nr
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|old_size
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object
modifier|*
name|base
init|=
name|old_hash
index|[
name|i
index|]
operator|.
name|base
decl_stmt|;
name|void
modifier|*
name|decoration
init|=
name|old_hash
index|[
name|i
index|]
operator|.
name|decoration
decl_stmt|;
if|if
condition|(
operator|!
name|base
condition|)
continue|continue;
name|insert_decoration
argument_list|(
name|n
argument_list|,
name|base
argument_list|,
name|decoration
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|old_hash
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Add a decoration pointer, return any old one */
end_comment
begin_function
DECL|function|add_decoration
name|void
modifier|*
name|add_decoration
parameter_list|(
name|struct
name|decoration
modifier|*
name|n
parameter_list|,
name|struct
name|object
modifier|*
name|obj
parameter_list|,
name|void
modifier|*
name|decoration
parameter_list|)
block|{
name|int
name|nr
init|=
name|n
operator|->
name|nr
operator|+
literal|1
decl_stmt|;
if|if
condition|(
name|nr
operator|>
name|n
operator|->
name|size
operator|*
literal|2
operator|/
literal|3
condition|)
name|grow_decoration
argument_list|(
name|n
argument_list|)
expr_stmt|;
return|return
name|insert_decoration
argument_list|(
name|n
argument_list|,
name|obj
argument_list|,
name|decoration
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/* Lookup a decoration pointer */
end_comment
begin_function
DECL|function|lookup_decoration
name|void
modifier|*
name|lookup_decoration
parameter_list|(
name|struct
name|decoration
modifier|*
name|n
parameter_list|,
name|struct
name|object
modifier|*
name|obj
parameter_list|)
block|{
name|int
name|j
decl_stmt|;
comment|/* nothing to lookup */
if|if
condition|(
operator|!
name|n
operator|->
name|size
condition|)
return|return
name|NULL
return|;
name|j
operator|=
name|hash_obj
argument_list|(
name|obj
argument_list|,
name|n
operator|->
name|size
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|struct
name|object_decoration
modifier|*
name|ref
init|=
name|n
operator|->
name|hash
operator|+
name|j
decl_stmt|;
if|if
condition|(
name|ref
operator|->
name|base
operator|==
name|obj
condition|)
return|return
name|ref
operator|->
name|decoration
return|;
if|if
condition|(
operator|!
name|ref
operator|->
name|base
condition|)
return|return
name|NULL
return|;
if|if
condition|(
operator|++
name|j
operator|==
name|n
operator|->
name|size
condition|)
name|j
operator|=
literal|0
expr_stmt|;
block|}
block|}
end_function
end_unit
