begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_function
DECL|function|prefixcmp
name|int
name|prefixcmp
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
for|for
control|(
init|;
condition|;
name|str
operator|++
operator|,
name|prefix
operator|++
control|)
if|if
condition|(
operator|!
operator|*
name|prefix
condition|)
return|return
literal|0
return|;
elseif|else
if|if
condition|(
operator|*
name|str
operator|!=
operator|*
name|prefix
condition|)
return|return
operator|(
name|unsigned
name|char
operator|)
operator|*
name|prefix
operator|-
operator|(
name|unsigned
name|char
operator|)
operator|*
name|str
return|;
block|}
end_function
begin_function
DECL|function|suffixcmp
name|int
name|suffixcmp
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
specifier|const
name|char
modifier|*
name|suffix
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|str
argument_list|)
decl_stmt|,
name|suflen
init|=
name|strlen
argument_list|(
name|suffix
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|<
name|suflen
condition|)
return|return
operator|-
literal|1
return|;
else|else
return|return
name|strcmp
argument_list|(
name|str
operator|+
name|len
operator|-
name|suflen
argument_list|,
name|suffix
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/*  * Used as the default ->buf value, so that people can always assume  * buf is non NULL and ->buf is NUL terminated even for a freshly  * initialized strbuf.  */
end_comment
begin_decl_stmt
DECL|variable|strbuf_slopbuf
name|char
name|strbuf_slopbuf
index|[
literal|1
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|strbuf_init
name|void
name|strbuf_init
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|size_t
name|hint
parameter_list|)
block|{
name|sb
operator|->
name|alloc
operator|=
name|sb
operator|->
name|len
operator|=
literal|0
expr_stmt|;
name|sb
operator|->
name|buf
operator|=
name|strbuf_slopbuf
expr_stmt|;
if|if
condition|(
name|hint
condition|)
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|hint
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_release
name|void
name|strbuf_release
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|)
block|{
if|if
condition|(
name|sb
operator|->
name|alloc
condition|)
block|{
name|free
argument_list|(
name|sb
operator|->
name|buf
argument_list|)
expr_stmt|;
name|strbuf_init
argument_list|(
name|sb
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|strbuf_detach
name|char
modifier|*
name|strbuf_detach
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|size_t
modifier|*
name|sz
parameter_list|)
block|{
name|char
modifier|*
name|res
decl_stmt|;
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|res
operator|=
name|sb
operator|->
name|buf
expr_stmt|;
if|if
condition|(
name|sz
condition|)
operator|*
name|sz
operator|=
name|sb
operator|->
name|len
expr_stmt|;
name|strbuf_init
argument_list|(
name|sb
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
name|res
return|;
block|}
end_function
begin_function
DECL|function|strbuf_attach
name|void
name|strbuf_attach
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|void
modifier|*
name|buf
parameter_list|,
name|size_t
name|len
parameter_list|,
name|size_t
name|alloc
parameter_list|)
block|{
name|strbuf_release
argument_list|(
name|sb
argument_list|)
expr_stmt|;
name|sb
operator|->
name|buf
operator|=
name|buf
expr_stmt|;
name|sb
operator|->
name|len
operator|=
name|len
expr_stmt|;
name|sb
operator|->
name|alloc
operator|=
name|alloc
expr_stmt|;
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_grow
name|void
name|strbuf_grow
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|size_t
name|extra
parameter_list|)
block|{
name|int
name|new_buf
init|=
operator|!
name|sb
operator|->
name|alloc
decl_stmt|;
if|if
condition|(
name|unsigned_add_overflows
argument_list|(
name|extra
argument_list|,
literal|1
argument_list|)
operator|||
name|unsigned_add_overflows
argument_list|(
name|sb
operator|->
name|len
argument_list|,
name|extra
operator|+
literal|1
argument_list|)
condition|)
name|die
argument_list|(
literal|"you want to use way too much memory"
argument_list|)
expr_stmt|;
if|if
condition|(
name|new_buf
condition|)
name|sb
operator|->
name|buf
operator|=
name|NULL
expr_stmt|;
name|ALLOC_GROW
argument_list|(
name|sb
operator|->
name|buf
argument_list|,
name|sb
operator|->
name|len
operator|+
name|extra
operator|+
literal|1
argument_list|,
name|sb
operator|->
name|alloc
argument_list|)
expr_stmt|;
if|if
condition|(
name|new_buf
condition|)
name|sb
operator|->
name|buf
index|[
literal|0
index|]
operator|=
literal|'\0'
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_trim
name|void
name|strbuf_trim
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|)
block|{
name|char
modifier|*
name|b
init|=
name|sb
operator|->
name|buf
decl_stmt|;
while|while
condition|(
name|sb
operator|->
name|len
operator|>
literal|0
operator|&&
name|isspace
argument_list|(
operator|(
name|unsigned
name|char
operator|)
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
operator|-
literal|1
index|]
argument_list|)
condition|)
name|sb
operator|->
name|len
operator|--
expr_stmt|;
while|while
condition|(
name|sb
operator|->
name|len
operator|>
literal|0
operator|&&
name|isspace
argument_list|(
operator|*
name|b
argument_list|)
condition|)
block|{
name|b
operator|++
expr_stmt|;
name|sb
operator|->
name|len
operator|--
expr_stmt|;
block|}
name|memmove
argument_list|(
name|sb
operator|->
name|buf
argument_list|,
name|b
argument_list|,
name|sb
operator|->
name|len
argument_list|)
expr_stmt|;
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_rtrim
name|void
name|strbuf_rtrim
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|)
block|{
while|while
condition|(
name|sb
operator|->
name|len
operator|>
literal|0
operator|&&
name|isspace
argument_list|(
operator|(
name|unsigned
name|char
operator|)
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
operator|-
literal|1
index|]
argument_list|)
condition|)
name|sb
operator|->
name|len
operator|--
expr_stmt|;
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_ltrim
name|void
name|strbuf_ltrim
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|)
block|{
name|char
modifier|*
name|b
init|=
name|sb
operator|->
name|buf
decl_stmt|;
while|while
condition|(
name|sb
operator|->
name|len
operator|>
literal|0
operator|&&
name|isspace
argument_list|(
operator|*
name|b
argument_list|)
condition|)
block|{
name|b
operator|++
expr_stmt|;
name|sb
operator|->
name|len
operator|--
expr_stmt|;
block|}
name|memmove
argument_list|(
name|sb
operator|->
name|buf
argument_list|,
name|b
argument_list|,
name|sb
operator|->
name|len
argument_list|)
expr_stmt|;
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_split_buf
name|struct
name|strbuf
modifier|*
modifier|*
name|strbuf_split_buf
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|size_t
name|slen
parameter_list|,
name|int
name|delim
parameter_list|,
name|int
name|max
parameter_list|)
block|{
name|int
name|alloc
init|=
literal|2
decl_stmt|,
name|pos
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|n
decl_stmt|,
modifier|*
name|p
decl_stmt|;
name|struct
name|strbuf
modifier|*
modifier|*
name|ret
decl_stmt|;
name|struct
name|strbuf
modifier|*
name|t
decl_stmt|;
name|ret
operator|=
name|xcalloc
argument_list|(
name|alloc
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|strbuf
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|p
operator|=
name|n
operator|=
name|str
expr_stmt|;
while|while
condition|(
name|n
operator|<
name|str
operator|+
name|slen
condition|)
block|{
name|int
name|len
decl_stmt|;
if|if
condition|(
name|max
operator|<=
literal|0
operator|||
name|pos
operator|+
literal|1
operator|<
name|max
condition|)
name|n
operator|=
name|memchr
argument_list|(
name|n
argument_list|,
name|delim
argument_list|,
name|slen
operator|-
operator|(
name|n
operator|-
name|str
operator|)
argument_list|)
expr_stmt|;
else|else
name|n
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|pos
operator|+
literal|1
operator|>=
name|alloc
condition|)
block|{
name|alloc
operator|=
name|alloc
operator|*
literal|2
expr_stmt|;
name|ret
operator|=
name|xrealloc
argument_list|(
name|ret
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|strbuf
operator|*
argument_list|)
operator|*
name|alloc
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|n
condition|)
name|n
operator|=
name|str
operator|+
name|slen
operator|-
literal|1
expr_stmt|;
name|len
operator|=
name|n
operator|-
name|p
operator|+
literal|1
expr_stmt|;
name|t
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|strbuf
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_init
argument_list|(
name|t
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
name|t
argument_list|,
name|p
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|ret
index|[
name|pos
index|]
operator|=
name|t
expr_stmt|;
name|ret
index|[
operator|++
name|pos
index|]
operator|=
name|NULL
expr_stmt|;
name|p
operator|=
operator|++
name|n
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|strbuf_list_free
name|void
name|strbuf_list_free
parameter_list|(
name|struct
name|strbuf
modifier|*
modifier|*
name|sbs
parameter_list|)
block|{
name|struct
name|strbuf
modifier|*
modifier|*
name|s
init|=
name|sbs
decl_stmt|;
while|while
condition|(
operator|*
name|s
condition|)
block|{
name|strbuf_release
argument_list|(
operator|*
name|s
argument_list|)
expr_stmt|;
name|free
argument_list|(
operator|*
name|s
operator|++
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|sbs
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_cmp
name|int
name|strbuf_cmp
parameter_list|(
specifier|const
name|struct
name|strbuf
modifier|*
name|a
parameter_list|,
specifier|const
name|struct
name|strbuf
modifier|*
name|b
parameter_list|)
block|{
name|int
name|len
init|=
name|a
operator|->
name|len
operator|<
name|b
operator|->
name|len
condition|?
name|a
operator|->
name|len
else|:
name|b
operator|->
name|len
decl_stmt|;
name|int
name|cmp
init|=
name|memcmp
argument_list|(
name|a
operator|->
name|buf
argument_list|,
name|b
operator|->
name|buf
argument_list|,
name|len
argument_list|)
decl_stmt|;
if|if
condition|(
name|cmp
condition|)
return|return
name|cmp
return|;
return|return
name|a
operator|->
name|len
operator|<
name|b
operator|->
name|len
condition|?
operator|-
literal|1
else|:
name|a
operator|->
name|len
operator|!=
name|b
operator|->
name|len
return|;
block|}
end_function
begin_function
DECL|function|strbuf_splice
name|void
name|strbuf_splice
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|size_t
name|pos
parameter_list|,
name|size_t
name|len
parameter_list|,
specifier|const
name|void
modifier|*
name|data
parameter_list|,
name|size_t
name|dlen
parameter_list|)
block|{
if|if
condition|(
name|unsigned_add_overflows
argument_list|(
name|pos
argument_list|,
name|len
argument_list|)
condition|)
name|die
argument_list|(
literal|"you want to use way too much memory"
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
operator|>
name|sb
operator|->
name|len
condition|)
name|die
argument_list|(
literal|"`pos' is too far after the end of the buffer"
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
operator|+
name|len
operator|>
name|sb
operator|->
name|len
condition|)
name|die
argument_list|(
literal|"`pos + len' is too far after the end of the buffer"
argument_list|)
expr_stmt|;
if|if
condition|(
name|dlen
operator|>=
name|len
condition|)
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|dlen
operator|-
name|len
argument_list|)
expr_stmt|;
name|memmove
argument_list|(
name|sb
operator|->
name|buf
operator|+
name|pos
operator|+
name|dlen
argument_list|,
name|sb
operator|->
name|buf
operator|+
name|pos
operator|+
name|len
argument_list|,
name|sb
operator|->
name|len
operator|-
name|pos
operator|-
name|len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|sb
operator|->
name|buf
operator|+
name|pos
argument_list|,
name|data
argument_list|,
name|dlen
argument_list|)
expr_stmt|;
name|strbuf_setlen
argument_list|(
name|sb
argument_list|,
name|sb
operator|->
name|len
operator|+
name|dlen
operator|-
name|len
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_insert
name|void
name|strbuf_insert
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|size_t
name|pos
parameter_list|,
specifier|const
name|void
modifier|*
name|data
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
name|strbuf_splice
argument_list|(
name|sb
argument_list|,
name|pos
argument_list|,
literal|0
argument_list|,
name|data
argument_list|,
name|len
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_remove
name|void
name|strbuf_remove
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|size_t
name|pos
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
name|strbuf_splice
argument_list|(
name|sb
argument_list|,
name|pos
argument_list|,
name|len
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_add
name|void
name|strbuf_add
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|void
modifier|*
name|data
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|sb
operator|->
name|buf
operator|+
name|sb
operator|->
name|len
argument_list|,
name|data
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|strbuf_setlen
argument_list|(
name|sb
argument_list|,
name|sb
operator|->
name|len
operator|+
name|len
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_adddup
name|void
name|strbuf_adddup
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|size_t
name|pos
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|sb
operator|->
name|buf
operator|+
name|sb
operator|->
name|len
argument_list|,
name|sb
operator|->
name|buf
operator|+
name|pos
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|strbuf_setlen
argument_list|(
name|sb
argument_list|,
name|sb
operator|->
name|len
operator|+
name|len
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_addf
name|void
name|strbuf_addf
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
name|va_list
name|ap
decl_stmt|;
name|va_start
argument_list|(
name|ap
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|strbuf_vaddf
argument_list|(
name|sb
argument_list|,
name|fmt
argument_list|,
name|ap
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|ap
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_vaddf
name|void
name|strbuf_vaddf
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
name|va_list
name|ap
parameter_list|)
block|{
name|int
name|len
decl_stmt|;
name|va_list
name|cp
decl_stmt|;
if|if
condition|(
operator|!
name|strbuf_avail
argument_list|(
name|sb
argument_list|)
condition|)
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
literal|64
argument_list|)
expr_stmt|;
name|va_copy
argument_list|(
name|cp
argument_list|,
name|ap
argument_list|)
expr_stmt|;
name|len
operator|=
name|vsnprintf
argument_list|(
name|sb
operator|->
name|buf
operator|+
name|sb
operator|->
name|len
argument_list|,
name|sb
operator|->
name|alloc
operator|-
name|sb
operator|->
name|len
argument_list|,
name|fmt
argument_list|,
name|cp
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|cp
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"BUG: your vsnprintf is broken (returned %d)"
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>
name|strbuf_avail
argument_list|(
name|sb
argument_list|)
condition|)
block|{
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|len
operator|=
name|vsnprintf
argument_list|(
name|sb
operator|->
name|buf
operator|+
name|sb
operator|->
name|len
argument_list|,
name|sb
operator|->
name|alloc
operator|-
name|sb
operator|->
name|len
argument_list|,
name|fmt
argument_list|,
name|ap
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>
name|strbuf_avail
argument_list|(
name|sb
argument_list|)
condition|)
name|die
argument_list|(
literal|"BUG: your vsnprintf is broken (insatiable)"
argument_list|)
expr_stmt|;
block|}
name|strbuf_setlen
argument_list|(
name|sb
argument_list|,
name|sb
operator|->
name|len
operator|+
name|len
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_expand
name|void
name|strbuf_expand
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|format
parameter_list|,
name|expand_fn_t
name|fn
parameter_list|,
name|void
modifier|*
name|context
parameter_list|)
block|{
for|for
control|(
init|;
condition|;
control|)
block|{
specifier|const
name|char
modifier|*
name|percent
decl_stmt|;
name|size_t
name|consumed
decl_stmt|;
name|percent
operator|=
name|strchrnul
argument_list|(
name|format
argument_list|,
literal|'%'
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
name|sb
argument_list|,
name|format
argument_list|,
name|percent
operator|-
name|format
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|*
name|percent
condition|)
break|break;
name|format
operator|=
name|percent
operator|+
literal|1
expr_stmt|;
if|if
condition|(
operator|*
name|format
operator|==
literal|'%'
condition|)
block|{
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
literal|'%'
argument_list|)
expr_stmt|;
name|format
operator|++
expr_stmt|;
continue|continue;
block|}
name|consumed
operator|=
name|fn
argument_list|(
name|sb
argument_list|,
name|format
argument_list|,
name|context
argument_list|)
expr_stmt|;
if|if
condition|(
name|consumed
condition|)
name|format
operator|+=
name|consumed
expr_stmt|;
else|else
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
literal|'%'
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|strbuf_expand_dict_cb
name|size_t
name|strbuf_expand_dict_cb
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|placeholder
parameter_list|,
name|void
modifier|*
name|context
parameter_list|)
block|{
name|struct
name|strbuf_expand_dict_entry
modifier|*
name|e
init|=
name|context
decl_stmt|;
name|size_t
name|len
decl_stmt|;
for|for
control|(
init|;
name|e
operator|->
name|placeholder
operator|&&
operator|(
name|len
operator|=
name|strlen
argument_list|(
name|e
operator|->
name|placeholder
argument_list|)
operator|)
condition|;
name|e
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|placeholder
argument_list|,
name|e
operator|->
name|placeholder
argument_list|,
name|len
argument_list|)
condition|)
block|{
if|if
condition|(
name|e
operator|->
name|value
condition|)
name|strbuf_addstr
argument_list|(
name|sb
argument_list|,
name|e
operator|->
name|value
argument_list|)
expr_stmt|;
return|return
name|len
return|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|strbuf_addbuf_percentquote
name|void
name|strbuf_addbuf_percentquote
parameter_list|(
name|struct
name|strbuf
modifier|*
name|dst
parameter_list|,
specifier|const
name|struct
name|strbuf
modifier|*
name|src
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|len
init|=
name|src
operator|->
name|len
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
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|src
operator|->
name|buf
index|[
name|i
index|]
operator|==
literal|'%'
condition|)
name|strbuf_addch
argument_list|(
name|dst
argument_list|,
literal|'%'
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
name|dst
argument_list|,
name|src
operator|->
name|buf
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|strbuf_fread
name|size_t
name|strbuf_fread
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|size_t
name|size
parameter_list|,
name|FILE
modifier|*
name|f
parameter_list|)
block|{
name|size_t
name|res
decl_stmt|;
name|size_t
name|oldalloc
init|=
name|sb
operator|->
name|alloc
decl_stmt|;
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|res
operator|=
name|fread
argument_list|(
name|sb
operator|->
name|buf
operator|+
name|sb
operator|->
name|len
argument_list|,
literal|1
argument_list|,
name|size
argument_list|,
name|f
argument_list|)
expr_stmt|;
if|if
condition|(
name|res
operator|>
literal|0
condition|)
name|strbuf_setlen
argument_list|(
name|sb
argument_list|,
name|sb
operator|->
name|len
operator|+
name|res
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|oldalloc
operator|==
literal|0
condition|)
name|strbuf_release
argument_list|(
name|sb
argument_list|)
expr_stmt|;
return|return
name|res
return|;
block|}
end_function
begin_function
DECL|function|strbuf_read
name|ssize_t
name|strbuf_read
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|int
name|fd
parameter_list|,
name|size_t
name|hint
parameter_list|)
block|{
name|size_t
name|oldlen
init|=
name|sb
operator|->
name|len
decl_stmt|;
name|size_t
name|oldalloc
init|=
name|sb
operator|->
name|alloc
decl_stmt|;
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|hint
condition|?
name|hint
else|:
literal|8192
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|ssize_t
name|cnt
decl_stmt|;
name|cnt
operator|=
name|xread
argument_list|(
name|fd
argument_list|,
name|sb
operator|->
name|buf
operator|+
name|sb
operator|->
name|len
argument_list|,
name|sb
operator|->
name|alloc
operator|-
name|sb
operator|->
name|len
operator|-
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|cnt
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|oldalloc
operator|==
literal|0
condition|)
name|strbuf_release
argument_list|(
name|sb
argument_list|)
expr_stmt|;
else|else
name|strbuf_setlen
argument_list|(
name|sb
argument_list|,
name|oldlen
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
operator|!
name|cnt
condition|)
break|break;
name|sb
operator|->
name|len
operator|+=
name|cnt
expr_stmt|;
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
literal|8192
argument_list|)
expr_stmt|;
block|}
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
return|return
name|sb
operator|->
name|len
operator|-
name|oldlen
return|;
block|}
end_function
begin_define
DECL|macro|STRBUF_MAXLINK
define|#
directive|define
name|STRBUF_MAXLINK
value|(2*PATH_MAX)
end_define
begin_function
DECL|function|strbuf_readlink
name|int
name|strbuf_readlink
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|size_t
name|hint
parameter_list|)
block|{
name|size_t
name|oldalloc
init|=
name|sb
operator|->
name|alloc
decl_stmt|;
if|if
condition|(
name|hint
operator|<
literal|32
condition|)
name|hint
operator|=
literal|32
expr_stmt|;
while|while
condition|(
name|hint
operator|<
name|STRBUF_MAXLINK
condition|)
block|{
name|int
name|len
decl_stmt|;
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|hint
argument_list|)
expr_stmt|;
name|len
operator|=
name|readlink
argument_list|(
name|path
argument_list|,
name|sb
operator|->
name|buf
argument_list|,
name|hint
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|ERANGE
condition|)
break|break;
block|}
elseif|else
if|if
condition|(
name|len
operator|<
name|hint
condition|)
block|{
name|strbuf_setlen
argument_list|(
name|sb
argument_list|,
name|len
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
comment|/* .. the buffer was too small - try again */
name|hint
operator|*=
literal|2
expr_stmt|;
block|}
if|if
condition|(
name|oldalloc
operator|==
literal|0
condition|)
name|strbuf_release
argument_list|(
name|sb
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|strbuf_getwholeline
name|int
name|strbuf_getwholeline
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|FILE
modifier|*
name|fp
parameter_list|,
name|int
name|term
parameter_list|)
block|{
name|int
name|ch
decl_stmt|;
if|if
condition|(
name|feof
argument_list|(
name|fp
argument_list|)
condition|)
return|return
name|EOF
return|;
name|strbuf_reset
argument_list|(
name|sb
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|ch
operator|=
name|fgetc
argument_list|(
name|fp
argument_list|)
operator|)
operator|!=
name|EOF
condition|)
block|{
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
operator|++
index|]
operator|=
name|ch
expr_stmt|;
if|if
condition|(
name|ch
operator|==
name|term
condition|)
break|break;
block|}
if|if
condition|(
name|ch
operator|==
name|EOF
operator|&&
name|sb
operator|->
name|len
operator|==
literal|0
condition|)
return|return
name|EOF
return|;
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|strbuf_getline
name|int
name|strbuf_getline
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|FILE
modifier|*
name|fp
parameter_list|,
name|int
name|term
parameter_list|)
block|{
if|if
condition|(
name|strbuf_getwholeline
argument_list|(
name|sb
argument_list|,
name|fp
argument_list|,
name|term
argument_list|)
condition|)
return|return
name|EOF
return|;
if|if
condition|(
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
operator|-
literal|1
index|]
operator|==
name|term
condition|)
name|strbuf_setlen
argument_list|(
name|sb
argument_list|,
name|sb
operator|->
name|len
operator|-
literal|1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|strbuf_getwholeline_fd
name|int
name|strbuf_getwholeline_fd
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|int
name|fd
parameter_list|,
name|int
name|term
parameter_list|)
block|{
name|strbuf_reset
argument_list|(
name|sb
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|char
name|ch
decl_stmt|;
name|ssize_t
name|len
init|=
name|xread
argument_list|(
name|fd
argument_list|,
operator|&
name|ch
argument_list|,
literal|1
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|<=
literal|0
condition|)
return|return
name|EOF
return|;
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
name|ch
argument_list|)
expr_stmt|;
if|if
condition|(
name|ch
operator|==
name|term
condition|)
break|break;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|strbuf_read_file
name|int
name|strbuf_read_file
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|size_t
name|hint
parameter_list|)
block|{
name|int
name|fd
decl_stmt|,
name|len
decl_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|path
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|len
operator|=
name|strbuf_read
argument_list|(
name|sb
argument_list|,
name|fd
argument_list|,
name|hint
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
return|return
name|len
return|;
block|}
end_function
begin_function
DECL|function|strbuf_add_lines
name|void
name|strbuf_add_lines
parameter_list|(
name|struct
name|strbuf
modifier|*
name|out
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|buf
parameter_list|,
name|size_t
name|size
parameter_list|)
block|{
while|while
condition|(
name|size
condition|)
block|{
specifier|const
name|char
modifier|*
name|next
init|=
name|memchr
argument_list|(
name|buf
argument_list|,
literal|'\n'
argument_list|,
name|size
argument_list|)
decl_stmt|;
name|next
operator|=
name|next
condition|?
operator|(
name|next
operator|+
literal|1
operator|)
else|:
operator|(
name|buf
operator|+
name|size
operator|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
name|out
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
name|out
argument_list|,
name|buf
argument_list|,
name|next
operator|-
name|buf
argument_list|)
expr_stmt|;
name|size
operator|-=
name|next
operator|-
name|buf
expr_stmt|;
name|buf
operator|=
name|next
expr_stmt|;
block|}
name|strbuf_complete_line
argument_list|(
name|out
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|is_rfc3986_reserved
specifier|static
name|int
name|is_rfc3986_reserved
parameter_list|(
name|char
name|ch
parameter_list|)
block|{
switch|switch
condition|(
name|ch
condition|)
block|{
case|case
literal|'!'
case|:
case|case
literal|'*'
case|:
case|case
literal|'\''
case|:
case|case
literal|'('
case|:
case|case
literal|')'
case|:
case|case
literal|';'
case|:
case|case
literal|':'
case|:
case|case
literal|'@'
case|:
case|case
literal|'&'
case|:
case|case
literal|'='
case|:
case|case
literal|'+'
case|:
case|case
literal|'$'
case|:
case|case
literal|','
case|:
case|case
literal|'/'
case|:
case|case
literal|'?'
case|:
case|case
literal|'#'
case|:
case|case
literal|'['
case|:
case|case
literal|']'
case|:
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|is_rfc3986_unreserved
specifier|static
name|int
name|is_rfc3986_unreserved
parameter_list|(
name|char
name|ch
parameter_list|)
block|{
return|return
name|isalnum
argument_list|(
name|ch
argument_list|)
operator|||
name|ch
operator|==
literal|'-'
operator|||
name|ch
operator|==
literal|'_'
operator|||
name|ch
operator|==
literal|'.'
operator|||
name|ch
operator|==
literal|'~'
return|;
block|}
end_function
begin_function
DECL|function|strbuf_add_urlencode
name|void
name|strbuf_add_urlencode
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|,
name|size_t
name|len
parameter_list|,
name|int
name|reserved
parameter_list|)
block|{
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|len
argument_list|)
expr_stmt|;
while|while
condition|(
name|len
operator|--
condition|)
block|{
name|char
name|ch
init|=
operator|*
name|s
operator|++
decl_stmt|;
if|if
condition|(
name|is_rfc3986_unreserved
argument_list|(
name|ch
argument_list|)
operator|||
operator|(
operator|!
name|reserved
operator|&&
name|is_rfc3986_reserved
argument_list|(
name|ch
argument_list|)
operator|)
condition|)
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
name|ch
argument_list|)
expr_stmt|;
else|else
name|strbuf_addf
argument_list|(
name|sb
argument_list|,
literal|"%%%02x"
argument_list|,
name|ch
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|strbuf_addstr_urlencode
name|void
name|strbuf_addstr_urlencode
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|,
name|int
name|reserved
parameter_list|)
block|{
name|strbuf_add_urlencode
argument_list|(
name|sb
argument_list|,
name|s
argument_list|,
name|strlen
argument_list|(
name|s
argument_list|)
argument_list|,
name|reserved
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
