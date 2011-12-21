begin_unit
begin_comment
comment|/*  * Licensed under a two-clause BSD-style license.  * See LICENSE for details.  */
end_comment
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"trp.h"
end_include
begin_include
include|#
directive|include
file|"obj_pool.h"
end_include
begin_include
include|#
directive|include
file|"string_pool.h"
end_include
begin_decl_stmt
DECL|variable|tree
specifier|static
name|struct
name|trp_root
name|tree
init|=
block|{
operator|~
literal|0U
block|}
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|node
struct|struct
name|node
block|{
DECL|member|offset
name|uint32_t
name|offset
decl_stmt|;
DECL|member|children
name|struct
name|trp_node
name|children
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/* Two memory pools: one for struct node, and another for strings */
end_comment
begin_macro
name|obj_pool_gen
argument_list|(
argument|node
argument_list|,
argument|struct node
argument_list|,
literal|4096
argument_list|)
end_macro
begin_macro
name|obj_pool_gen
argument_list|(
argument|string
argument_list|,
argument|char
argument_list|,
literal|4096
argument_list|)
end_macro
begin_function
DECL|function|node_value
specifier|static
name|char
modifier|*
name|node_value
parameter_list|(
name|struct
name|node
modifier|*
name|node
parameter_list|)
block|{
return|return
name|node
condition|?
name|string_pointer
argument_list|(
name|node
operator|->
name|offset
argument_list|)
else|:
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|node_cmp
specifier|static
name|int
name|node_cmp
parameter_list|(
name|struct
name|node
modifier|*
name|a
parameter_list|,
name|struct
name|node
modifier|*
name|b
parameter_list|)
block|{
return|return
name|strcmp
argument_list|(
name|node_value
argument_list|(
name|a
argument_list|)
argument_list|,
name|node_value
argument_list|(
name|b
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/* Build a Treap from the node structure (a trp_node w/ offset) */
end_comment
begin_macro
name|trp_gen
argument_list|(
argument|static
argument_list|,
argument|tree_
argument_list|,
argument|struct node
argument_list|,
argument|children
argument_list|,
argument|node
argument_list|,
argument|node_cmp
argument_list|)
end_macro
begin_function
DECL|function|pool_fetch
specifier|const
name|char
modifier|*
name|pool_fetch
parameter_list|(
name|uint32_t
name|entry
parameter_list|)
block|{
return|return
name|node_value
argument_list|(
name|node_pointer
argument_list|(
name|entry
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|pool_intern
name|uint32_t
name|pool_intern
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|)
block|{
comment|/* Canonicalize key */
name|struct
name|node
modifier|*
name|match
init|=
name|NULL
decl_stmt|,
modifier|*
name|node
decl_stmt|;
name|uint32_t
name|key_len
decl_stmt|;
if|if
condition|(
name|key
operator|==
name|NULL
condition|)
return|return
operator|~
literal|0
return|;
name|key_len
operator|=
name|strlen
argument_list|(
name|key
argument_list|)
operator|+
literal|1
expr_stmt|;
name|node
operator|=
name|node_pointer
argument_list|(
name|node_alloc
argument_list|(
literal|1
argument_list|)
argument_list|)
expr_stmt|;
name|node
operator|->
name|offset
operator|=
name|string_alloc
argument_list|(
name|key_len
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|node_value
argument_list|(
name|node
argument_list|)
argument_list|,
name|key
argument_list|)
expr_stmt|;
name|match
operator|=
name|tree_search
argument_list|(
operator|&
name|tree
argument_list|,
name|node
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|match
condition|)
block|{
name|tree_insert
argument_list|(
operator|&
name|tree
argument_list|,
name|node
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|node_free
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|string_free
argument_list|(
name|key_len
argument_list|)
expr_stmt|;
name|node
operator|=
name|match
expr_stmt|;
block|}
return|return
name|node_offset
argument_list|(
name|node
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|pool_tok_r
name|uint32_t
name|pool_tok_r
parameter_list|(
name|char
modifier|*
name|str
parameter_list|,
specifier|const
name|char
modifier|*
name|delim
parameter_list|,
name|char
modifier|*
modifier|*
name|saveptr
parameter_list|)
block|{
name|char
modifier|*
name|token
init|=
name|strtok_r
argument_list|(
name|str
argument_list|,
name|delim
argument_list|,
name|saveptr
argument_list|)
decl_stmt|;
return|return
name|token
condition|?
name|pool_intern
argument_list|(
name|token
argument_list|)
else|:
operator|~
literal|0
return|;
block|}
end_function
begin_function
DECL|function|pool_print_seq
name|void
name|pool_print_seq
parameter_list|(
name|uint32_t
name|len
parameter_list|,
name|uint32_t
modifier|*
name|seq
parameter_list|,
name|char
name|delim
parameter_list|,
name|FILE
modifier|*
name|stream
parameter_list|)
block|{
name|uint32_t
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
name|len
operator|&&
operator|~
name|seq
index|[
name|i
index|]
condition|;
name|i
operator|++
control|)
block|{
name|fputs
argument_list|(
name|pool_fetch
argument_list|(
name|seq
index|[
name|i
index|]
argument_list|)
argument_list|,
name|stream
argument_list|)
expr_stmt|;
if|if
condition|(
name|i
operator|<
name|len
operator|-
literal|1
operator|&&
operator|~
name|seq
index|[
name|i
operator|+
literal|1
index|]
condition|)
name|fputc
argument_list|(
name|delim
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|pool_tok_seq
name|uint32_t
name|pool_tok_seq
parameter_list|(
name|uint32_t
name|sz
parameter_list|,
name|uint32_t
modifier|*
name|seq
parameter_list|,
specifier|const
name|char
modifier|*
name|delim
parameter_list|,
name|char
modifier|*
name|str
parameter_list|)
block|{
name|char
modifier|*
name|context
init|=
name|NULL
decl_stmt|;
name|uint32_t
name|token
init|=
operator|~
literal|0U
decl_stmt|;
name|uint32_t
name|length
decl_stmt|;
if|if
condition|(
name|sz
operator|==
literal|0
condition|)
return|return
operator|~
literal|0
return|;
if|if
condition|(
name|str
condition|)
name|token
operator|=
name|pool_tok_r
argument_list|(
name|str
argument_list|,
name|delim
argument_list|,
operator|&
name|context
argument_list|)
expr_stmt|;
for|for
control|(
name|length
operator|=
literal|0
init|;
name|length
operator|<
name|sz
condition|;
name|length
operator|++
control|)
block|{
name|seq
index|[
name|length
index|]
operator|=
name|token
expr_stmt|;
if|if
condition|(
name|token
operator|==
operator|~
literal|0
condition|)
return|return
name|length
return|;
name|token
operator|=
name|pool_tok_r
argument_list|(
name|NULL
argument_list|,
name|delim
argument_list|,
operator|&
name|context
argument_list|)
expr_stmt|;
block|}
name|seq
index|[
name|sz
operator|-
literal|1
index|]
operator|=
operator|~
literal|0
expr_stmt|;
return|return
name|sz
return|;
block|}
end_function
begin_function
DECL|function|pool_reset
name|void
name|pool_reset
parameter_list|(
name|void
parameter_list|)
block|{
name|node_reset
argument_list|()
expr_stmt|;
name|string_reset
argument_list|()
expr_stmt|;
block|}
end_function
end_unit
