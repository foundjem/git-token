begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"remote.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"walker.h"
end_include
begin_include
include|#
directive|include
file|"http.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_decl_stmt
DECL|variable|remote
specifier|static
name|struct
name|remote
modifier|*
name|remote
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|url
specifier|static
specifier|const
name|char
modifier|*
name|url
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|walker
specifier|static
name|struct
name|walker
modifier|*
name|walker
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|init_walker
specifier|static
name|void
name|init_walker
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|walker
condition|)
name|walker
operator|=
name|get_http_walker
argument_list|(
name|url
argument_list|,
name|remote
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_refs
specifier|static
name|struct
name|ref
modifier|*
name|get_refs
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|strbuf
name|buffer
init|=
name|STRBUF_INIT
decl_stmt|;
name|char
modifier|*
name|data
decl_stmt|,
modifier|*
name|start
decl_stmt|,
modifier|*
name|mid
decl_stmt|;
name|char
modifier|*
name|ref_name
decl_stmt|;
name|char
modifier|*
name|refs_url
decl_stmt|;
name|int
name|i
init|=
literal|0
decl_stmt|;
name|int
name|http_ret
decl_stmt|;
name|struct
name|ref
modifier|*
name|refs
init|=
name|NULL
decl_stmt|;
name|struct
name|ref
modifier|*
name|ref
init|=
name|NULL
decl_stmt|;
name|struct
name|ref
modifier|*
name|last_ref
init|=
name|NULL
decl_stmt|;
name|refs_url
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|url
argument_list|)
operator|+
literal|11
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|refs_url
argument_list|,
literal|"%s/info/refs"
argument_list|,
name|url
argument_list|)
expr_stmt|;
name|init_walker
argument_list|()
expr_stmt|;
name|http_ret
operator|=
name|http_get_strbuf
argument_list|(
name|refs_url
argument_list|,
operator|&
name|buffer
argument_list|,
name|HTTP_NO_CACHE
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|http_ret
condition|)
block|{
case|case
name|HTTP_OK
case|:
break|break;
case|case
name|HTTP_MISSING_TARGET
case|:
name|die
argument_list|(
literal|"%s not found: did you run git update-server-info on the"
literal|" server?"
argument_list|,
name|refs_url
argument_list|)
expr_stmt|;
default|default:
name|http_error
argument_list|(
name|refs_url
argument_list|,
name|http_ret
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"HTTP request failed"
argument_list|)
expr_stmt|;
block|}
name|data
operator|=
name|buffer
operator|.
name|buf
expr_stmt|;
name|start
operator|=
name|NULL
expr_stmt|;
name|mid
operator|=
name|data
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|buffer
operator|.
name|len
condition|)
block|{
if|if
condition|(
operator|!
name|start
condition|)
block|{
name|start
operator|=
operator|&
name|data
index|[
name|i
index|]
expr_stmt|;
block|}
if|if
condition|(
name|data
index|[
name|i
index|]
operator|==
literal|'\t'
condition|)
name|mid
operator|=
operator|&
name|data
index|[
name|i
index|]
expr_stmt|;
if|if
condition|(
name|data
index|[
name|i
index|]
operator|==
literal|'\n'
condition|)
block|{
name|data
index|[
name|i
index|]
operator|=
literal|0
expr_stmt|;
name|ref_name
operator|=
name|mid
operator|+
literal|1
expr_stmt|;
name|ref
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|ref
argument_list|)
operator|+
name|strlen
argument_list|(
name|ref_name
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|ref
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|ref
argument_list|)
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|ref
operator|->
name|name
argument_list|,
name|ref_name
argument_list|)
expr_stmt|;
name|get_sha1_hex
argument_list|(
name|start
argument_list|,
name|ref
operator|->
name|old_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|refs
condition|)
name|refs
operator|=
name|ref
expr_stmt|;
if|if
condition|(
name|last_ref
condition|)
name|last_ref
operator|->
name|next
operator|=
name|ref
expr_stmt|;
name|last_ref
operator|=
name|ref
expr_stmt|;
name|start
operator|=
name|NULL
expr_stmt|;
block|}
name|i
operator|++
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
name|ref
operator|=
name|alloc_ref
argument_list|(
literal|"HEAD"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|walker
operator|->
name|fetch_ref
argument_list|(
name|walker
argument_list|,
name|ref
argument_list|)
operator|&&
operator|!
name|resolve_remote_symref
argument_list|(
name|ref
argument_list|,
name|refs
argument_list|)
condition|)
block|{
name|ref
operator|->
name|next
operator|=
name|refs
expr_stmt|;
name|refs
operator|=
name|ref
expr_stmt|;
block|}
else|else
block|{
name|free
argument_list|(
name|ref
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|refs_url
argument_list|)
expr_stmt|;
return|return
name|refs
return|;
block|}
end_function
begin_function
DECL|function|fetch_dumb
specifier|static
name|int
name|fetch_dumb
parameter_list|(
name|int
name|nr_heads
parameter_list|,
name|struct
name|ref
modifier|*
modifier|*
name|to_fetch
parameter_list|)
block|{
name|char
modifier|*
modifier|*
name|targets
init|=
name|xmalloc
argument_list|(
name|nr_heads
operator|*
sizeof|sizeof
argument_list|(
name|char
operator|*
argument_list|)
argument_list|)
decl_stmt|;
name|int
name|ret
decl_stmt|,
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
name|nr_heads
condition|;
name|i
operator|++
control|)
name|targets
index|[
name|i
index|]
operator|=
name|xstrdup
argument_list|(
name|sha1_to_hex
argument_list|(
name|to_fetch
index|[
name|i
index|]
operator|->
name|old_sha1
argument_list|)
argument_list|)
expr_stmt|;
name|init_walker
argument_list|()
expr_stmt|;
name|walker
operator|->
name|get_all
operator|=
literal|1
expr_stmt|;
name|walker
operator|->
name|get_tree
operator|=
literal|1
expr_stmt|;
name|walker
operator|->
name|get_history
operator|=
literal|1
expr_stmt|;
name|walker
operator|->
name|get_verbosely
operator|=
literal|0
expr_stmt|;
name|walker
operator|->
name|get_recover
operator|=
literal|0
expr_stmt|;
name|ret
operator|=
name|walker_fetch
argument_list|(
name|walker
argument_list|,
name|nr_heads
argument_list|,
name|targets
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_heads
condition|;
name|i
operator|++
control|)
name|free
argument_list|(
name|targets
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|targets
argument_list|)
expr_stmt|;
return|return
name|ret
condition|?
name|error
argument_list|(
literal|"Fetch failed."
argument_list|)
else|:
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_fetch
specifier|static
name|void
name|parse_fetch
parameter_list|(
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|)
block|{
name|struct
name|ref
modifier|*
modifier|*
name|to_fetch
init|=
name|NULL
decl_stmt|;
name|struct
name|ref
modifier|*
name|list_head
init|=
name|NULL
decl_stmt|;
name|struct
name|ref
modifier|*
modifier|*
name|list
init|=
operator|&
name|list_head
decl_stmt|;
name|int
name|alloc_heads
init|=
literal|0
decl_stmt|,
name|nr_heads
init|=
literal|0
decl_stmt|;
do|do
block|{
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|buf
operator|->
name|buf
argument_list|,
literal|"fetch "
argument_list|)
condition|)
block|{
name|char
modifier|*
name|p
init|=
name|buf
operator|->
name|buf
operator|+
name|strlen
argument_list|(
literal|"fetch "
argument_list|)
decl_stmt|;
name|char
modifier|*
name|name
decl_stmt|;
name|struct
name|ref
modifier|*
name|ref
decl_stmt|;
name|unsigned
name|char
name|old_sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|strlen
argument_list|(
name|p
argument_list|)
operator|<
literal|40
operator|||
name|get_sha1_hex
argument_list|(
name|p
argument_list|,
name|old_sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"protocol error: expected sha/ref, got %s'"
argument_list|,
name|p
argument_list|)
expr_stmt|;
if|if
condition|(
name|p
index|[
literal|40
index|]
operator|==
literal|' '
condition|)
name|name
operator|=
name|p
operator|+
literal|41
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|p
index|[
literal|40
index|]
condition|)
name|name
operator|=
literal|""
expr_stmt|;
else|else
name|die
argument_list|(
literal|"protocol error: expected sha/ref, got %s'"
argument_list|,
name|p
argument_list|)
expr_stmt|;
name|ref
operator|=
name|alloc_ref
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|,
name|old_sha1
argument_list|)
expr_stmt|;
operator|*
name|list
operator|=
name|ref
expr_stmt|;
name|list
operator|=
operator|&
name|ref
operator|->
name|next
expr_stmt|;
name|ALLOC_GROW
argument_list|(
name|to_fetch
argument_list|,
name|nr_heads
operator|+
literal|1
argument_list|,
name|alloc_heads
argument_list|)
expr_stmt|;
name|to_fetch
index|[
name|nr_heads
operator|++
index|]
operator|=
name|ref
expr_stmt|;
block|}
else|else
name|die
argument_list|(
literal|"http transport does not support %s"
argument_list|,
name|buf
operator|->
name|buf
argument_list|)
expr_stmt|;
name|strbuf_reset
argument_list|(
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|strbuf_getline
argument_list|(
name|buf
argument_list|,
name|stdin
argument_list|,
literal|'\n'
argument_list|)
operator|==
name|EOF
condition|)
return|return;
if|if
condition|(
operator|!
operator|*
name|buf
operator|->
name|buf
condition|)
break|break;
block|}
do|while
condition|(
literal|1
condition|)
do|;
if|if
condition|(
name|fetch_dumb
argument_list|(
name|nr_heads
argument_list|,
name|to_fetch
argument_list|)
condition|)
name|exit
argument_list|(
literal|128
argument_list|)
expr_stmt|;
comment|/* error already reported */
name|free_refs
argument_list|(
name|list_head
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|to_fetch
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
name|strbuf_reset
argument_list|(
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|git_extract_argv0_path
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|setup_git_directory
argument_list|()
expr_stmt|;
if|if
condition|(
name|argc
operator|<
literal|2
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Remote needed\n"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|remote
operator|=
name|remote_get
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|>
literal|2
condition|)
block|{
name|url
operator|=
name|argv
index|[
literal|2
index|]
expr_stmt|;
block|}
else|else
block|{
name|url
operator|=
name|remote
operator|->
name|url
index|[
literal|0
index|]
expr_stmt|;
block|}
do|do
block|{
if|if
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|buf
argument_list|,
name|stdin
argument_list|,
literal|'\n'
argument_list|)
operator|==
name|EOF
condition|)
break|break;
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
literal|"fetch "
argument_list|)
condition|)
block|{
name|parse_fetch
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
literal|"list"
argument_list|)
condition|)
block|{
name|struct
name|ref
modifier|*
name|refs
init|=
name|get_refs
argument_list|()
decl_stmt|;
name|struct
name|ref
modifier|*
name|posn
decl_stmt|;
for|for
control|(
name|posn
operator|=
name|refs
init|;
name|posn
condition|;
name|posn
operator|=
name|posn
operator|->
name|next
control|)
block|{
if|if
condition|(
name|posn
operator|->
name|symref
condition|)
name|printf
argument_list|(
literal|"@%s %s\n"
argument_list|,
name|posn
operator|->
name|symref
argument_list|,
name|posn
operator|->
name|name
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|posn
operator|->
name|old_sha1
argument_list|)
argument_list|,
name|posn
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
literal|"capabilities"
argument_list|)
condition|)
block|{
name|printf
argument_list|(
literal|"fetch\n"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
block|}
else|else
block|{
return|return
literal|1
return|;
block|}
name|strbuf_reset
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
do|while
condition|(
literal|1
condition|)
do|;
return|return
literal|0
return|;
block|}
end_function
end_unit
