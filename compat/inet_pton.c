begin_unit
begin_comment
comment|/*  * Copyright (C) 1996-2001  Internet Software Consortium.  *  * Permission to use, copy, modify, and distribute this software for any  * purpose with or without fee is hereby granted, provided that the above  * copyright notice and this permission notice appear in all copies.  *  * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM  * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL  * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,  * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING  * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,  * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  */
end_comment
begin_include
include|#
directive|include
file|"../git-compat-util.h"
end_include
begin_ifndef
ifndef|#
directive|ifndef
name|NS_INT16SZ
end_ifndef
begin_define
DECL|macro|NS_INT16SZ
define|#
directive|define
name|NS_INT16SZ
value|2
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_ifndef
ifndef|#
directive|ifndef
name|NS_INADDRSZ
end_ifndef
begin_define
DECL|macro|NS_INADDRSZ
define|#
directive|define
name|NS_INADDRSZ
value|4
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_ifndef
ifndef|#
directive|ifndef
name|NS_IN6ADDRSZ
end_ifndef
begin_define
DECL|macro|NS_IN6ADDRSZ
define|#
directive|define
name|NS_IN6ADDRSZ
value|16
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/*  * WARNING: Don't even consider trying to compile this on a system where  * sizeof(int)< 4.  sizeof(int)> 4 is fine; all the world's not a VAX.  */
end_comment
begin_function_decl
specifier|static
name|int
name|inet_pton4
parameter_list|(
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|unsigned
name|char
modifier|*
name|dst
parameter_list|)
function_decl|;
end_function_decl
begin_ifndef
ifndef|#
directive|ifndef
name|NO_IPV6
end_ifndef
begin_function_decl
specifier|static
name|int
name|inet_pton6
parameter_list|(
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|unsigned
name|char
modifier|*
name|dst
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* int  * inet_pton4(src, dst)  *      like inet_aton() but without all the hexadecimal and shorthand.  * return:  *      1 if `src' is a valid dotted quad, else 0.  * notice:  *      does not touch `dst' unless it's returning 1.  * author:  *      Paul Vixie, 1996.  */
end_comment
begin_function
specifier|static
name|int
DECL|function|inet_pton4
name|inet_pton4
parameter_list|(
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|unsigned
name|char
modifier|*
name|dst
parameter_list|)
block|{
specifier|static
specifier|const
name|char
name|digits
index|[]
init|=
literal|"0123456789"
decl_stmt|;
name|int
name|saw_digit
decl_stmt|,
name|octets
decl_stmt|,
name|ch
decl_stmt|;
name|unsigned
name|char
name|tmp
index|[
name|NS_INADDRSZ
index|]
decl_stmt|,
modifier|*
name|tp
decl_stmt|;
name|saw_digit
operator|=
literal|0
expr_stmt|;
name|octets
operator|=
literal|0
expr_stmt|;
operator|*
operator|(
name|tp
operator|=
name|tmp
operator|)
operator|=
literal|0
expr_stmt|;
while|while
condition|(
operator|(
name|ch
operator|=
operator|*
name|src
operator|++
operator|)
operator|!=
literal|'\0'
condition|)
block|{
specifier|const
name|char
modifier|*
name|pch
decl_stmt|;
if|if
condition|(
operator|(
name|pch
operator|=
name|strchr
argument_list|(
name|digits
argument_list|,
name|ch
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|unsigned
name|int
name|new
init|=
operator|*
name|tp
operator|*
literal|10
operator|+
operator|(
name|pch
operator|-
name|digits
operator|)
decl_stmt|;
if|if
condition|(
name|new
operator|>
literal|255
condition|)
return|return
operator|(
literal|0
operator|)
return|;
operator|*
name|tp
operator|=
name|new
expr_stmt|;
if|if
condition|(
operator|!
name|saw_digit
condition|)
block|{
if|if
condition|(
operator|++
name|octets
operator|>
literal|4
condition|)
return|return
operator|(
literal|0
operator|)
return|;
name|saw_digit
operator|=
literal|1
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|ch
operator|==
literal|'.'
operator|&&
name|saw_digit
condition|)
block|{
if|if
condition|(
name|octets
operator|==
literal|4
condition|)
return|return
operator|(
literal|0
operator|)
return|;
operator|*
operator|++
name|tp
operator|=
literal|0
expr_stmt|;
name|saw_digit
operator|=
literal|0
expr_stmt|;
block|}
else|else
return|return
operator|(
literal|0
operator|)
return|;
block|}
if|if
condition|(
name|octets
operator|<
literal|4
condition|)
return|return
operator|(
literal|0
operator|)
return|;
name|memcpy
argument_list|(
name|dst
argument_list|,
name|tmp
argument_list|,
name|NS_INADDRSZ
argument_list|)
expr_stmt|;
return|return
operator|(
literal|1
operator|)
return|;
block|}
end_function
begin_comment
comment|/* int  * inet_pton6(src, dst)  *      convert presentation level address to network order binary form.  * return:  *      1 if `src' is a valid [RFC1884 2.2] address, else 0.  * notice:  *      (1) does not touch `dst' unless it's returning 1.  *      (2) :: in a full address is silently ignored.  * credit:  *      inspired by Mark Andrews.  * author:  *      Paul Vixie, 1996.  */
end_comment
begin_ifndef
ifndef|#
directive|ifndef
name|NO_IPV6
end_ifndef
begin_function
specifier|static
name|int
DECL|function|inet_pton6
name|inet_pton6
parameter_list|(
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|unsigned
name|char
modifier|*
name|dst
parameter_list|)
block|{
specifier|static
specifier|const
name|char
name|xdigits_l
index|[]
init|=
literal|"0123456789abcdef"
decl_stmt|,
name|xdigits_u
index|[]
init|=
literal|"0123456789ABCDEF"
decl_stmt|;
name|unsigned
name|char
name|tmp
index|[
name|NS_IN6ADDRSZ
index|]
decl_stmt|,
modifier|*
name|tp
decl_stmt|,
modifier|*
name|endp
decl_stmt|,
modifier|*
name|colonp
decl_stmt|;
specifier|const
name|char
modifier|*
name|xdigits
decl_stmt|,
modifier|*
name|curtok
decl_stmt|;
name|int
name|ch
decl_stmt|,
name|saw_xdigit
decl_stmt|;
name|unsigned
name|int
name|val
decl_stmt|;
name|memset
argument_list|(
operator|(
name|tp
operator|=
name|tmp
operator|)
argument_list|,
literal|'\0'
argument_list|,
name|NS_IN6ADDRSZ
argument_list|)
expr_stmt|;
name|endp
operator|=
name|tp
operator|+
name|NS_IN6ADDRSZ
expr_stmt|;
name|colonp
operator|=
name|NULL
expr_stmt|;
comment|/* Leading :: requires some special handling. */
if|if
condition|(
operator|*
name|src
operator|==
literal|':'
condition|)
if|if
condition|(
operator|*
operator|++
name|src
operator|!=
literal|':'
condition|)
return|return
operator|(
literal|0
operator|)
return|;
name|curtok
operator|=
name|src
expr_stmt|;
name|saw_xdigit
operator|=
literal|0
expr_stmt|;
name|val
operator|=
literal|0
expr_stmt|;
while|while
condition|(
operator|(
name|ch
operator|=
operator|*
name|src
operator|++
operator|)
operator|!=
literal|'\0'
condition|)
block|{
specifier|const
name|char
modifier|*
name|pch
decl_stmt|;
if|if
condition|(
operator|(
name|pch
operator|=
name|strchr
argument_list|(
operator|(
name|xdigits
operator|=
name|xdigits_l
operator|)
argument_list|,
name|ch
argument_list|)
operator|)
operator|==
name|NULL
condition|)
name|pch
operator|=
name|strchr
argument_list|(
operator|(
name|xdigits
operator|=
name|xdigits_u
operator|)
argument_list|,
name|ch
argument_list|)
expr_stmt|;
if|if
condition|(
name|pch
operator|!=
name|NULL
condition|)
block|{
name|val
operator|<<=
literal|4
expr_stmt|;
name|val
operator||=
operator|(
name|pch
operator|-
name|xdigits
operator|)
expr_stmt|;
if|if
condition|(
name|val
operator|>
literal|0xffff
condition|)
return|return
operator|(
literal|0
operator|)
return|;
name|saw_xdigit
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|ch
operator|==
literal|':'
condition|)
block|{
name|curtok
operator|=
name|src
expr_stmt|;
if|if
condition|(
operator|!
name|saw_xdigit
condition|)
block|{
if|if
condition|(
name|colonp
condition|)
return|return
operator|(
literal|0
operator|)
return|;
name|colonp
operator|=
name|tp
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|tp
operator|+
name|NS_INT16SZ
operator|>
name|endp
condition|)
return|return
operator|(
literal|0
operator|)
return|;
operator|*
name|tp
operator|++
operator|=
call|(
name|unsigned
name|char
call|)
argument_list|(
name|val
operator|>>
literal|8
argument_list|)
operator|&
literal|0xff
expr_stmt|;
operator|*
name|tp
operator|++
operator|=
operator|(
name|unsigned
name|char
operator|)
name|val
operator|&
literal|0xff
expr_stmt|;
name|saw_xdigit
operator|=
literal|0
expr_stmt|;
name|val
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|ch
operator|==
literal|'.'
operator|&&
operator|(
operator|(
name|tp
operator|+
name|NS_INADDRSZ
operator|)
operator|<=
name|endp
operator|)
operator|&&
name|inet_pton4
argument_list|(
name|curtok
argument_list|,
name|tp
argument_list|)
operator|>
literal|0
condition|)
block|{
name|tp
operator|+=
name|NS_INADDRSZ
expr_stmt|;
name|saw_xdigit
operator|=
literal|0
expr_stmt|;
break|break;
comment|/* '\0' was seen by inet_pton4(). */
block|}
return|return
operator|(
literal|0
operator|)
return|;
block|}
if|if
condition|(
name|saw_xdigit
condition|)
block|{
if|if
condition|(
name|tp
operator|+
name|NS_INT16SZ
operator|>
name|endp
condition|)
return|return
operator|(
literal|0
operator|)
return|;
operator|*
name|tp
operator|++
operator|=
call|(
name|unsigned
name|char
call|)
argument_list|(
name|val
operator|>>
literal|8
argument_list|)
operator|&
literal|0xff
expr_stmt|;
operator|*
name|tp
operator|++
operator|=
operator|(
name|unsigned
name|char
operator|)
name|val
operator|&
literal|0xff
expr_stmt|;
block|}
if|if
condition|(
name|colonp
operator|!=
name|NULL
condition|)
block|{
comment|/*                  * Since some memmove()'s erroneously fail to handle                  * overlapping regions, we'll do the shift by hand.                  */
specifier|const
name|int
name|n
init|=
name|tp
operator|-
name|colonp
decl_stmt|;
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<=
name|n
condition|;
name|i
operator|++
control|)
block|{
name|endp
index|[
operator|-
name|i
index|]
operator|=
name|colonp
index|[
name|n
operator|-
name|i
index|]
expr_stmt|;
name|colonp
index|[
name|n
operator|-
name|i
index|]
operator|=
literal|0
expr_stmt|;
block|}
name|tp
operator|=
name|endp
expr_stmt|;
block|}
if|if
condition|(
name|tp
operator|!=
name|endp
condition|)
return|return
operator|(
literal|0
operator|)
return|;
name|memcpy
argument_list|(
name|dst
argument_list|,
name|tmp
argument_list|,
name|NS_IN6ADDRSZ
argument_list|)
expr_stmt|;
return|return
operator|(
literal|1
operator|)
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* int  * isc_net_pton(af, src, dst)  *      convert from presentation format (which usually means ASCII printable)  *      to network format (which is usually some kind of binary format).  * return:  *      1 if the address was valid for the specified address family  *      0 if the address wasn't valid (`dst' is untouched in this case)  *      -1 if some other error occurred (`dst' is untouched in this case, too)  * author:  *      Paul Vixie, 1996.  */
end_comment
begin_function
name|int
DECL|function|inet_pton
name|inet_pton
parameter_list|(
name|int
name|af
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|void
modifier|*
name|dst
parameter_list|)
block|{
switch|switch
condition|(
name|af
condition|)
block|{
case|case
name|AF_INET
case|:
return|return
operator|(
name|inet_pton4
argument_list|(
name|src
argument_list|,
name|dst
argument_list|)
operator|)
return|;
ifndef|#
directive|ifndef
name|NO_IPV6
case|case
name|AF_INET6
case|:
return|return
operator|(
name|inet_pton6
argument_list|(
name|src
argument_list|,
name|dst
argument_list|)
operator|)
return|;
endif|#
directive|endif
default|default:
name|errno
operator|=
name|EAFNOSUPPORT
expr_stmt|;
return|return
operator|(
operator|-
literal|1
operator|)
return|;
block|}
comment|/* NOTREACHED */
block|}
end_function
end_unit
