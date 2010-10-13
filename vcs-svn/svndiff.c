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
file|"line_buffer.h"
end_include
begin_include
include|#
directive|include
file|"svndiff.h"
end_include
begin_comment
comment|/*  * svndiff0 applier  *  * See http://svn.apache.org/repos/asf/subversion/trunk/notes/svndiff.  *  * svndiff0 ::= 'SVN\0' window*  * window ::= int int int int int instructions inline_data;  * int ::= highdigit* lowdigit;  * highdigit ::= # binary 1000 0000 OR-ed with 7 bit value;  * lowdigit ::= # 7 bit value;  */
end_comment
begin_define
DECL|macro|VLI_CONTINUE
define|#
directive|define
name|VLI_CONTINUE
value|0x80
end_define
begin_define
DECL|macro|VLI_DIGIT_MASK
define|#
directive|define
name|VLI_DIGIT_MASK
value|0x7f
end_define
begin_define
DECL|macro|VLI_BITS_PER_DIGIT
define|#
directive|define
name|VLI_BITS_PER_DIGIT
value|7
end_define
begin_function
DECL|function|error_short_read
specifier|static
name|int
name|error_short_read
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|input
parameter_list|)
block|{
if|if
condition|(
name|buffer_ferror
argument_list|(
name|input
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"error reading delta: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
return|return
name|error
argument_list|(
literal|"invalid delta: unexpected end of file"
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|read_magic
specifier|static
name|int
name|read_magic
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|in
parameter_list|,
name|off_t
modifier|*
name|len
parameter_list|)
block|{
specifier|static
specifier|const
name|char
name|magic
index|[]
init|=
block|{
literal|'S'
block|,
literal|'V'
block|,
literal|'N'
block|,
literal|'\0'
block|}
decl_stmt|;
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
operator|*
name|len
operator|<
sizeof|sizeof
argument_list|(
name|magic
argument_list|)
operator|||
name|buffer_read_binary
argument_list|(
name|in
argument_list|,
operator|&
name|sb
argument_list|,
sizeof|sizeof
argument_list|(
name|magic
argument_list|)
argument_list|)
operator|!=
sizeof|sizeof
argument_list|(
name|magic
argument_list|)
condition|)
block|{
name|error_short_read
argument_list|(
name|in
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|memcmp
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
name|magic
argument_list|,
sizeof|sizeof
argument_list|(
name|magic
argument_list|)
argument_list|)
condition|)
block|{
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"invalid delta: unrecognized file type"
argument_list|)
return|;
block|}
operator|*
name|len
operator|-=
sizeof|sizeof
argument_list|(
name|magic
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_int
specifier|static
name|int
name|read_int
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|in
parameter_list|,
name|uintmax_t
modifier|*
name|result
parameter_list|,
name|off_t
modifier|*
name|len
parameter_list|)
block|{
name|uintmax_t
name|rv
init|=
literal|0
decl_stmt|;
name|off_t
name|sz
decl_stmt|;
for|for
control|(
name|sz
operator|=
operator|*
name|len
init|;
name|sz
condition|;
name|sz
operator|--
control|)
block|{
specifier|const
name|int
name|ch
init|=
name|buffer_read_char
argument_list|(
name|in
argument_list|)
decl_stmt|;
if|if
condition|(
name|ch
operator|==
name|EOF
condition|)
break|break;
name|rv
operator|<<=
name|VLI_BITS_PER_DIGIT
expr_stmt|;
name|rv
operator|+=
operator|(
name|ch
operator|&
name|VLI_DIGIT_MASK
operator|)
expr_stmt|;
if|if
condition|(
name|ch
operator|&
name|VLI_CONTINUE
condition|)
continue|continue;
operator|*
name|result
operator|=
name|rv
expr_stmt|;
operator|*
name|len
operator|=
name|sz
operator|-
literal|1
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
name|error_short_read
argument_list|(
name|in
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|read_offset
specifier|static
name|int
name|read_offset
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|in
parameter_list|,
name|off_t
modifier|*
name|result
parameter_list|,
name|off_t
modifier|*
name|len
parameter_list|)
block|{
name|uintmax_t
name|val
decl_stmt|;
if|if
condition|(
name|read_int
argument_list|(
name|in
argument_list|,
operator|&
name|val
argument_list|,
name|len
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|val
operator|>
name|maximum_signed_value_of_type
argument_list|(
name|off_t
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"unrepresentable offset in delta: %"
name|PRIuMAX
literal|""
argument_list|,
name|val
argument_list|)
return|;
operator|*
name|result
operator|=
name|val
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_length
specifier|static
name|int
name|read_length
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|in
parameter_list|,
name|size_t
modifier|*
name|result
parameter_list|,
name|off_t
modifier|*
name|len
parameter_list|)
block|{
name|uintmax_t
name|val
decl_stmt|;
if|if
condition|(
name|read_int
argument_list|(
name|in
argument_list|,
operator|&
name|val
argument_list|,
name|len
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|val
operator|>
name|SIZE_MAX
condition|)
return|return
name|error
argument_list|(
literal|"unrepresentable length in delta: %"
name|PRIuMAX
literal|""
argument_list|,
name|val
argument_list|)
return|;
operator|*
name|result
operator|=
name|val
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|apply_one_window
specifier|static
name|int
name|apply_one_window
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|delta
parameter_list|,
name|off_t
modifier|*
name|delta_len
parameter_list|)
block|{
name|size_t
name|out_len
decl_stmt|;
name|size_t
name|instructions_len
decl_stmt|;
name|size_t
name|data_len
decl_stmt|;
name|assert
argument_list|(
name|delta_len
argument_list|)
expr_stmt|;
comment|/* "source view" offset and length already handled; */
if|if
condition|(
name|read_length
argument_list|(
name|delta
argument_list|,
operator|&
name|out_len
argument_list|,
name|delta_len
argument_list|)
operator|||
name|read_length
argument_list|(
name|delta
argument_list|,
operator|&
name|instructions_len
argument_list|,
name|delta_len
argument_list|)
operator|||
name|read_length
argument_list|(
name|delta
argument_list|,
operator|&
name|data_len
argument_list|,
name|delta_len
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|instructions_len
condition|)
return|return
name|error
argument_list|(
literal|"What do you think I am?  A delta applier?"
argument_list|)
return|;
if|if
condition|(
name|data_len
condition|)
return|return
name|error
argument_list|(
literal|"No support for inline data yet"
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|svndiff0_apply
name|int
name|svndiff0_apply
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|delta
parameter_list|,
name|off_t
name|delta_len
parameter_list|,
name|struct
name|sliding_view
modifier|*
name|preimage
parameter_list|,
name|FILE
modifier|*
name|postimage
parameter_list|)
block|{
name|assert
argument_list|(
name|delta
operator|&&
name|preimage
operator|&&
name|postimage
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_magic
argument_list|(
name|delta
argument_list|,
operator|&
name|delta_len
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
while|while
condition|(
name|delta_len
condition|)
block|{
comment|/* For each window: */
name|off_t
name|pre_off
decl_stmt|;
name|size_t
name|pre_len
decl_stmt|;
if|if
condition|(
name|read_offset
argument_list|(
name|delta
argument_list|,
operator|&
name|pre_off
argument_list|,
operator|&
name|delta_len
argument_list|)
operator|||
name|read_length
argument_list|(
name|delta
argument_list|,
operator|&
name|pre_len
argument_list|,
operator|&
name|delta_len
argument_list|)
operator|||
name|apply_one_window
argument_list|(
name|delta
argument_list|,
operator|&
name|delta_len
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
