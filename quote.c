begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_comment
comment|/* Help to copy the thing properly quoted for the shell safety.  * any single quote is replaced with '\'', any exclamation point  * is replaced with '\!', and the whole thing is enclosed in a  *  * E.g.  *  original     sq_quote     result  *  name     ==> name      ==> 'name'  *  a b      ==> a b       ==> 'a b'  *  a'b      ==> a'\''b    ==> 'a'\''b'  *  a!b      ==> a'\!'b    ==> 'a'\!'b'  */
end_comment
begin_function
DECL|function|need_bs_quote
specifier|static
specifier|inline
name|int
name|need_bs_quote
parameter_list|(
name|char
name|c
parameter_list|)
block|{
return|return
operator|(
name|c
operator|==
literal|'\''
operator|||
name|c
operator|==
literal|'!'
operator|)
return|;
block|}
end_function
begin_function
DECL|function|sq_quote_buf
name|void
name|sq_quote_buf
parameter_list|(
name|struct
name|strbuf
modifier|*
name|dst
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|)
block|{
name|char
modifier|*
name|to_free
init|=
name|NULL
decl_stmt|;
if|if
condition|(
name|dst
operator|->
name|buf
operator|==
name|src
condition|)
name|to_free
operator|=
name|strbuf_detach
argument_list|(
name|dst
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
name|dst
argument_list|,
literal|'\''
argument_list|)
expr_stmt|;
while|while
condition|(
operator|*
name|src
condition|)
block|{
name|size_t
name|len
init|=
name|strcspn
argument_list|(
name|src
argument_list|,
literal|"'!"
argument_list|)
decl_stmt|;
name|strbuf_add
argument_list|(
name|dst
argument_list|,
name|src
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|src
operator|+=
name|len
expr_stmt|;
while|while
condition|(
name|need_bs_quote
argument_list|(
operator|*
name|src
argument_list|)
condition|)
block|{
name|strbuf_addstr
argument_list|(
name|dst
argument_list|,
literal|"'\\"
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
name|dst
argument_list|,
operator|*
name|src
operator|++
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
name|dst
argument_list|,
literal|'\''
argument_list|)
expr_stmt|;
block|}
block|}
name|strbuf_addch
argument_list|(
name|dst
argument_list|,
literal|'\''
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|to_free
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|sq_quote_print
name|void
name|sq_quote_print
parameter_list|(
name|FILE
modifier|*
name|stream
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|)
block|{
name|char
name|c
decl_stmt|;
name|fputc
argument_list|(
literal|'\''
argument_list|,
name|stream
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|src
operator|++
operator|)
condition|)
block|{
if|if
condition|(
name|need_bs_quote
argument_list|(
name|c
argument_list|)
condition|)
block|{
name|fputs
argument_list|(
literal|"'\\"
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fputc
argument_list|(
name|c
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fputc
argument_list|(
literal|'\''
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|fputc
argument_list|(
name|c
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
block|}
name|fputc
argument_list|(
literal|'\''
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|sq_quote_argv
name|void
name|sq_quote_argv
parameter_list|(
name|struct
name|strbuf
modifier|*
name|dst
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|size_t
name|maxlen
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
comment|/* Copy into destination buffer. */
name|strbuf_grow
argument_list|(
name|dst
argument_list|,
literal|255
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|argv
index|[
name|i
index|]
condition|;
operator|++
name|i
control|)
block|{
name|strbuf_addch
argument_list|(
name|dst
argument_list|,
literal|' '
argument_list|)
expr_stmt|;
name|sq_quote_buf
argument_list|(
name|dst
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|maxlen
operator|&&
name|dst
operator|->
name|len
operator|>
name|maxlen
condition|)
name|die
argument_list|(
literal|"Too many or long arguments"
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|sq_dequote
name|char
modifier|*
name|sq_dequote
parameter_list|(
name|char
modifier|*
name|arg
parameter_list|)
block|{
name|char
modifier|*
name|dst
init|=
name|arg
decl_stmt|;
name|char
modifier|*
name|src
init|=
name|arg
decl_stmt|;
name|char
name|c
decl_stmt|;
if|if
condition|(
operator|*
name|src
operator|!=
literal|'\''
condition|)
return|return
name|NULL
return|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|c
operator|=
operator|*
operator|++
name|src
expr_stmt|;
if|if
condition|(
operator|!
name|c
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|c
operator|!=
literal|'\''
condition|)
block|{
operator|*
name|dst
operator|++
operator|=
name|c
expr_stmt|;
continue|continue;
block|}
comment|/* We stepped out of sq */
switch|switch
condition|(
operator|*
operator|++
name|src
condition|)
block|{
case|case
literal|'\0'
case|:
operator|*
name|dst
operator|=
literal|0
expr_stmt|;
return|return
name|arg
return|;
case|case
literal|'\\'
case|:
name|c
operator|=
operator|*
operator|++
name|src
expr_stmt|;
if|if
condition|(
name|need_bs_quote
argument_list|(
name|c
argument_list|)
operator|&&
operator|*
operator|++
name|src
operator|==
literal|'\''
condition|)
block|{
operator|*
name|dst
operator|++
operator|=
name|c
expr_stmt|;
continue|continue;
block|}
comment|/* Fallthrough */
default|default:
return|return
name|NULL
return|;
block|}
block|}
block|}
end_function
begin_comment
comment|/* 1 means: quote as octal  * 0 means: quote as octal if (quote_path_fully)  * -1 means: never quote  * c: quote as "\\c"  */
end_comment
begin_define
DECL|macro|X8
define|#
directive|define
name|X8
parameter_list|(
name|x
parameter_list|)
value|x, x, x, x, x, x, x, x
end_define
begin_define
DECL|macro|X16
define|#
directive|define
name|X16
parameter_list|(
name|x
parameter_list|)
value|X8(x), X8(x)
end_define
begin_decl_stmt
DECL|variable|sq_lookup
specifier|static
name|signed
name|char
specifier|const
name|sq_lookup
index|[
literal|256
index|]
init|=
block|{
comment|/*           0    1    2    3    4    5    6    7 */
comment|/* 0x00 */
literal|1
block|,
literal|1
block|,
literal|1
block|,
literal|1
block|,
literal|1
block|,
literal|1
block|,
literal|1
block|,
literal|'a'
block|,
comment|/* 0x08 */
literal|'b'
block|,
literal|'t'
block|,
literal|'n'
block|,
literal|'v'
block|,
literal|'f'
block|,
literal|'r'
block|,
literal|1
block|,
literal|1
block|,
comment|/* 0x10 */
name|X16
argument_list|(
literal|1
argument_list|)
block|,
comment|/* 0x20 */
operator|-
literal|1
block|,
operator|-
literal|1
block|,
literal|'"'
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
comment|/* 0x28 */
name|X16
argument_list|(
operator|-
literal|1
argument_list|)
block|,
name|X16
argument_list|(
operator|-
literal|1
argument_list|)
block|,
name|X16
argument_list|(
operator|-
literal|1
argument_list|)
block|,
comment|/* 0x58 */
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
literal|'\\'
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
comment|/* 0x60 */
name|X16
argument_list|(
operator|-
literal|1
argument_list|)
block|,
name|X8
argument_list|(
operator|-
literal|1
argument_list|)
block|,
comment|/* 0x78 */
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
literal|1
block|,
comment|/* 0x80 */
comment|/* set to 0 */
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|sq_must_quote
specifier|static
specifier|inline
name|int
name|sq_must_quote
parameter_list|(
name|char
name|c
parameter_list|)
block|{
return|return
name|sq_lookup
index|[
operator|(
name|unsigned
name|char
operator|)
name|c
index|]
operator|+
name|quote_path_fully
operator|>
literal|0
return|;
block|}
end_function
begin_comment
comment|/* returns the longest prefix not needing a quote up to maxlen if positive.    This stops at the first \0 because it's marked as a character needing an    escape */
end_comment
begin_function
DECL|function|next_quote_pos
specifier|static
name|size_t
name|next_quote_pos
parameter_list|(
specifier|const
name|char
modifier|*
name|s
parameter_list|,
name|ssize_t
name|maxlen
parameter_list|)
block|{
name|size_t
name|len
decl_stmt|;
if|if
condition|(
name|maxlen
operator|<
literal|0
condition|)
block|{
for|for
control|(
name|len
operator|=
literal|0
init|;
operator|!
name|sq_must_quote
argument_list|(
name|s
index|[
name|len
index|]
argument_list|)
condition|;
name|len
operator|++
control|)
empty_stmt|;
block|}
else|else
block|{
for|for
control|(
name|len
operator|=
literal|0
init|;
name|len
operator|<
name|maxlen
operator|&&
operator|!
name|sq_must_quote
argument_list|(
name|s
index|[
name|len
index|]
argument_list|)
condition|;
name|len
operator|++
control|)
empty_stmt|;
block|}
return|return
name|len
return|;
block|}
end_function
begin_comment
comment|/*  * C-style name quoting.  *  * (1) if sb and fp are both NULL, inspect the input name and counts the  *     number of bytes that are needed to hold c_style quoted version of name,  *     counting the double quotes around it but not terminating NUL, and  *     returns it.  *     However, if name does not need c_style quoting, it returns 0.  *  * (2) if sb or fp are not NULL, it emits the c_style quoted version  *     of name, enclosed with double quotes if asked and needed only.  *     Return value is the same as in (1).  */
end_comment
begin_function
DECL|function|quote_c_style_counted
specifier|static
name|size_t
name|quote_c_style_counted
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|ssize_t
name|maxlen
parameter_list|,
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
name|no_dq
parameter_list|)
block|{
DECL|macro|EMIT
undef|#
directive|undef
name|EMIT
DECL|macro|EMIT
define|#
directive|define
name|EMIT
parameter_list|(
name|c
parameter_list|)
define|\
value|do {                                        \ 		if (sb) strbuf_addch(sb, (c));          \ 		if (fp) fputc((c), fp);                 \ 		count++;                                \ 	} while (0)
DECL|macro|EMITBUF
define|#
directive|define
name|EMITBUF
parameter_list|(
name|s
parameter_list|,
name|l
parameter_list|)
define|\
value|do {                                        \ 		if (sb) strbuf_add(sb, (s), (l));       \ 		if (fp) fwrite((s), (l), 1, fp);        \ 		count += (l);                           \ 	} while (0)
name|size_t
name|len
decl_stmt|,
name|count
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|p
init|=
name|name
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|ch
decl_stmt|;
name|len
operator|=
name|next_quote_pos
argument_list|(
name|p
argument_list|,
name|maxlen
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|==
name|maxlen
operator|||
operator|!
name|p
index|[
name|len
index|]
condition|)
break|break;
if|if
condition|(
operator|!
name|no_dq
operator|&&
name|p
operator|==
name|name
condition|)
name|EMIT
argument_list|(
literal|'"'
argument_list|)
expr_stmt|;
name|EMITBUF
argument_list|(
name|p
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|EMIT
argument_list|(
literal|'\\'
argument_list|)
expr_stmt|;
name|p
operator|+=
name|len
expr_stmt|;
name|ch
operator|=
operator|(
name|unsigned
name|char
operator|)
operator|*
name|p
operator|++
expr_stmt|;
if|if
condition|(
name|sq_lookup
index|[
name|ch
index|]
operator|>=
literal|' '
condition|)
block|{
name|EMIT
argument_list|(
name|sq_lookup
index|[
name|ch
index|]
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|EMIT
argument_list|(
operator|(
operator|(
name|ch
operator|>>
literal|6
operator|)
operator|&
literal|03
operator|)
operator|+
literal|'0'
argument_list|)
expr_stmt|;
name|EMIT
argument_list|(
operator|(
operator|(
name|ch
operator|>>
literal|3
operator|)
operator|&
literal|07
operator|)
operator|+
literal|'0'
argument_list|)
expr_stmt|;
name|EMIT
argument_list|(
operator|(
operator|(
name|ch
operator|>>
literal|0
operator|)
operator|&
literal|07
operator|)
operator|+
literal|'0'
argument_list|)
expr_stmt|;
block|}
block|}
name|EMITBUF
argument_list|(
name|p
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|p
operator|==
name|name
condition|)
comment|/* no ending quote needed */
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|no_dq
condition|)
name|EMIT
argument_list|(
literal|'"'
argument_list|)
expr_stmt|;
return|return
name|count
return|;
block|}
end_function
begin_function
DECL|function|quote_c_style
name|size_t
name|quote_c_style
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
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
name|nodq
parameter_list|)
block|{
return|return
name|quote_c_style_counted
argument_list|(
name|name
argument_list|,
operator|-
literal|1
argument_list|,
name|sb
argument_list|,
name|fp
argument_list|,
name|nodq
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|quote_two_c_style
name|void
name|quote_two_c_style
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|nodq
parameter_list|)
block|{
if|if
condition|(
name|quote_c_style
argument_list|(
name|prefix
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
operator|||
name|quote_c_style
argument_list|(
name|path
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|nodq
condition|)
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
literal|'"'
argument_list|)
expr_stmt|;
name|quote_c_style
argument_list|(
name|prefix
argument_list|,
name|sb
argument_list|,
name|NULL
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|quote_c_style
argument_list|(
name|path
argument_list|,
name|sb
argument_list|,
name|NULL
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|nodq
condition|)
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
literal|'"'
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|strbuf_addstr
argument_list|(
name|sb
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
name|sb
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|write_name_quoted
name|void
name|write_name_quoted
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|FILE
modifier|*
name|fp
parameter_list|,
name|int
name|terminator
parameter_list|)
block|{
if|if
condition|(
name|terminator
condition|)
block|{
name|quote_c_style
argument_list|(
name|name
argument_list|,
name|NULL
argument_list|,
name|fp
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|fputs
argument_list|(
name|name
argument_list|,
name|fp
argument_list|)
expr_stmt|;
block|}
name|fputc
argument_list|(
name|terminator
argument_list|,
name|fp
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|write_name_quotedpfx
specifier|extern
name|void
name|write_name_quotedpfx
parameter_list|(
specifier|const
name|char
modifier|*
name|pfx
parameter_list|,
name|size_t
name|pfxlen
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|FILE
modifier|*
name|fp
parameter_list|,
name|int
name|terminator
parameter_list|)
block|{
name|int
name|needquote
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|terminator
condition|)
block|{
name|needquote
operator|=
name|next_quote_pos
argument_list|(
name|pfx
argument_list|,
name|pfxlen
argument_list|)
operator|<
name|pfxlen
operator|||
name|name
index|[
name|next_quote_pos
argument_list|(
name|name
argument_list|,
operator|-
literal|1
argument_list|)
index|]
expr_stmt|;
block|}
if|if
condition|(
name|needquote
condition|)
block|{
name|fputc
argument_list|(
literal|'"'
argument_list|,
name|fp
argument_list|)
expr_stmt|;
name|quote_c_style_counted
argument_list|(
name|pfx
argument_list|,
name|pfxlen
argument_list|,
name|NULL
argument_list|,
name|fp
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|quote_c_style
argument_list|(
name|name
argument_list|,
name|NULL
argument_list|,
name|fp
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|fputc
argument_list|(
literal|'"'
argument_list|,
name|fp
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|fwrite
argument_list|(
name|pfx
argument_list|,
name|pfxlen
argument_list|,
literal|1
argument_list|,
name|fp
argument_list|)
expr_stmt|;
name|fputs
argument_list|(
name|name
argument_list|,
name|fp
argument_list|)
expr_stmt|;
block|}
name|fputc
argument_list|(
name|terminator
argument_list|,
name|fp
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* quote path as relative to the given prefix */
end_comment
begin_function
DECL|function|quote_path_relative
name|char
modifier|*
name|quote_path_relative
parameter_list|(
specifier|const
name|char
modifier|*
name|in
parameter_list|,
name|int
name|len
parameter_list|,
name|struct
name|strbuf
modifier|*
name|out
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|int
name|needquote
decl_stmt|;
if|if
condition|(
name|len
operator|<
literal|0
condition|)
name|len
operator|=
name|strlen
argument_list|(
name|in
argument_list|)
expr_stmt|;
comment|/* "../" prefix itself does not need quoting, but "in" might. */
name|needquote
operator|=
name|next_quote_pos
argument_list|(
name|in
argument_list|,
name|len
argument_list|)
operator|<
name|len
expr_stmt|;
name|strbuf_setlen
argument_list|(
name|out
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strbuf_grow
argument_list|(
name|out
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|needquote
condition|)
name|strbuf_addch
argument_list|(
name|out
argument_list|,
literal|'"'
argument_list|)
expr_stmt|;
if|if
condition|(
name|prefix
condition|)
block|{
name|int
name|off
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|prefix
index|[
name|off
index|]
operator|&&
name|off
operator|<
name|len
operator|&&
name|prefix
index|[
name|off
index|]
operator|==
name|in
index|[
name|off
index|]
condition|)
if|if
condition|(
name|prefix
index|[
name|off
index|]
operator|==
literal|'/'
condition|)
block|{
name|prefix
operator|+=
name|off
operator|+
literal|1
expr_stmt|;
name|in
operator|+=
name|off
operator|+
literal|1
expr_stmt|;
name|len
operator|-=
name|off
operator|+
literal|1
expr_stmt|;
name|off
operator|=
literal|0
expr_stmt|;
block|}
else|else
name|off
operator|++
expr_stmt|;
for|for
control|(
init|;
operator|*
name|prefix
condition|;
name|prefix
operator|++
control|)
if|if
condition|(
operator|*
name|prefix
operator|==
literal|'/'
condition|)
name|strbuf_addstr
argument_list|(
name|out
argument_list|,
literal|"../"
argument_list|)
expr_stmt|;
block|}
name|quote_c_style_counted
argument_list|(
name|in
argument_list|,
name|len
argument_list|,
name|out
argument_list|,
name|NULL
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|needquote
condition|)
name|strbuf_addch
argument_list|(
name|out
argument_list|,
literal|'"'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|out
operator|->
name|len
condition|)
name|strbuf_addstr
argument_list|(
name|out
argument_list|,
literal|"./"
argument_list|)
expr_stmt|;
return|return
name|out
operator|->
name|buf
return|;
block|}
end_function
begin_comment
comment|/*  * C-style name unquoting.  *  * Quoted should point at the opening double quote.  * + Returns 0 if it was able to unquote the string properly, and appends the  *   result in the strbuf `sb'.  * + Returns -1 in case of error, and doesn't touch the strbuf. Though note  *   that this function will allocate memory in the strbuf, so calling  *   strbuf_release is mandatory whichever result unquote_c_style returns.  *  * Updates endp pointer to point at one past the ending double quote if given.  */
end_comment
begin_function
DECL|function|unquote_c_style
name|int
name|unquote_c_style
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|quoted
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|endp
parameter_list|)
block|{
name|size_t
name|oldlen
init|=
name|sb
operator|->
name|len
decl_stmt|,
name|len
decl_stmt|;
name|int
name|ch
decl_stmt|,
name|ac
decl_stmt|;
if|if
condition|(
operator|*
name|quoted
operator|++
operator|!=
literal|'"'
condition|)
return|return
operator|-
literal|1
return|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|len
operator|=
name|strcspn
argument_list|(
name|quoted
argument_list|,
literal|"\"\\"
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
name|sb
argument_list|,
name|quoted
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|quoted
operator|+=
name|len
expr_stmt|;
switch|switch
condition|(
operator|*
name|quoted
operator|++
condition|)
block|{
case|case
literal|'"'
case|:
if|if
condition|(
name|endp
condition|)
operator|*
name|endp
operator|=
name|quoted
operator|+
literal|1
expr_stmt|;
return|return
literal|0
return|;
case|case
literal|'\\'
case|:
break|break;
default|default:
goto|goto
name|error
goto|;
block|}
switch|switch
condition|(
operator|(
name|ch
operator|=
operator|*
name|quoted
operator|++
operator|)
condition|)
block|{
case|case
literal|'a'
case|:
name|ch
operator|=
literal|'\a'
expr_stmt|;
break|break;
case|case
literal|'b'
case|:
name|ch
operator|=
literal|'\b'
expr_stmt|;
break|break;
case|case
literal|'f'
case|:
name|ch
operator|=
literal|'\f'
expr_stmt|;
break|break;
case|case
literal|'n'
case|:
name|ch
operator|=
literal|'\n'
expr_stmt|;
break|break;
case|case
literal|'r'
case|:
name|ch
operator|=
literal|'\r'
expr_stmt|;
break|break;
case|case
literal|'t'
case|:
name|ch
operator|=
literal|'\t'
expr_stmt|;
break|break;
case|case
literal|'v'
case|:
name|ch
operator|=
literal|'\v'
expr_stmt|;
break|break;
case|case
literal|'\\'
case|:
case|case
literal|'"'
case|:
break|break;
comment|/* verbatim */
comment|/* octal values with first digit over 4 overflow */
case|case
literal|'0'
case|:
case|case
literal|'1'
case|:
case|case
literal|'2'
case|:
case|case
literal|'3'
case|:
name|ac
operator|=
operator|(
operator|(
name|ch
operator|-
literal|'0'
operator|)
operator|<<
literal|6
operator|)
expr_stmt|;
if|if
condition|(
operator|(
name|ch
operator|=
operator|*
name|quoted
operator|++
operator|)
operator|<
literal|'0'
operator|||
literal|'7'
operator|<
name|ch
condition|)
goto|goto
name|error
goto|;
name|ac
operator||=
operator|(
operator|(
name|ch
operator|-
literal|'0'
operator|)
operator|<<
literal|3
operator|)
expr_stmt|;
if|if
condition|(
operator|(
name|ch
operator|=
operator|*
name|quoted
operator|++
operator|)
operator|<
literal|'0'
operator|||
literal|'7'
operator|<
name|ch
condition|)
goto|goto
name|error
goto|;
name|ac
operator||=
operator|(
name|ch
operator|-
literal|'0'
operator|)
expr_stmt|;
name|ch
operator|=
name|ac
expr_stmt|;
break|break;
default|default:
goto|goto
name|error
goto|;
block|}
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
name|ch
argument_list|)
expr_stmt|;
block|}
name|error
label|:
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
end_function
begin_comment
comment|/* quoting as a string literal for other languages */
end_comment
begin_function
DECL|function|perl_quote_print
name|void
name|perl_quote_print
parameter_list|(
name|FILE
modifier|*
name|stream
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|)
block|{
specifier|const
name|char
name|sq
init|=
literal|'\''
decl_stmt|;
specifier|const
name|char
name|bq
init|=
literal|'\\'
decl_stmt|;
name|char
name|c
decl_stmt|;
name|fputc
argument_list|(
name|sq
argument_list|,
name|stream
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|src
operator|++
operator|)
condition|)
block|{
if|if
condition|(
name|c
operator|==
name|sq
operator|||
name|c
operator|==
name|bq
condition|)
name|fputc
argument_list|(
name|bq
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fputc
argument_list|(
name|c
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
name|fputc
argument_list|(
name|sq
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|python_quote_print
name|void
name|python_quote_print
parameter_list|(
name|FILE
modifier|*
name|stream
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|)
block|{
specifier|const
name|char
name|sq
init|=
literal|'\''
decl_stmt|;
specifier|const
name|char
name|bq
init|=
literal|'\\'
decl_stmt|;
specifier|const
name|char
name|nl
init|=
literal|'\n'
decl_stmt|;
name|char
name|c
decl_stmt|;
name|fputc
argument_list|(
name|sq
argument_list|,
name|stream
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|src
operator|++
operator|)
condition|)
block|{
if|if
condition|(
name|c
operator|==
name|nl
condition|)
block|{
name|fputc
argument_list|(
name|bq
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fputc
argument_list|(
literal|'n'
argument_list|,
name|stream
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|c
operator|==
name|sq
operator|||
name|c
operator|==
name|bq
condition|)
name|fputc
argument_list|(
name|bq
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fputc
argument_list|(
name|c
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
name|fputc
argument_list|(
name|sq
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|tcl_quote_print
name|void
name|tcl_quote_print
parameter_list|(
name|FILE
modifier|*
name|stream
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|)
block|{
name|char
name|c
decl_stmt|;
name|fputc
argument_list|(
literal|'"'
argument_list|,
name|stream
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|src
operator|++
operator|)
condition|)
block|{
switch|switch
condition|(
name|c
condition|)
block|{
case|case
literal|'['
case|:
case|case
literal|']'
case|:
case|case
literal|'{'
case|:
case|case
literal|'}'
case|:
case|case
literal|'$'
case|:
case|case
literal|'\\'
case|:
case|case
literal|'"'
case|:
name|fputc
argument_list|(
literal|'\\'
argument_list|,
name|stream
argument_list|)
expr_stmt|;
default|default:
name|fputc
argument_list|(
name|c
argument_list|,
name|stream
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'\f'
case|:
name|fputs
argument_list|(
literal|"\\f"
argument_list|,
name|stream
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'\r'
case|:
name|fputs
argument_list|(
literal|"\\r"
argument_list|,
name|stream
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'\n'
case|:
name|fputs
argument_list|(
literal|"\\n"
argument_list|,
name|stream
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'\t'
case|:
name|fputs
argument_list|(
literal|"\\t"
argument_list|,
name|stream
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'\v'
case|:
name|fputs
argument_list|(
literal|"\\v"
argument_list|,
name|stream
argument_list|)
expr_stmt|;
break|break;
block|}
block|}
name|fputc
argument_list|(
literal|'"'
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
