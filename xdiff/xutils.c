begin_unit
begin_comment
comment|/*  *  LibXDiff by Davide Libenzi ( File Differential Library )  *  Copyright (C) 2003	Davide Libenzi  *  *  This library is free software; you can redistribute it and/or  *  modify it under the terms of the GNU Lesser General Public  *  License as published by the Free Software Foundation; either  *  version 2.1 of the License, or (at your option) any later version.  *  *  This library is distributed in the hope that it will be useful,  *  but WITHOUT ANY WARRANTY; without even the implied warranty of  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *  Lesser General Public License for more details.  *  *  You should have received a copy of the GNU Lesser General Public  *  License along with this library; if not, write to the Free Software  *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  *  *  Davide Libenzi<davidel@xmailserver.org>  *  */
end_comment
begin_include
include|#
directive|include
file|"xinclude.h"
end_include
begin_define
DECL|macro|XDL_GUESS_NLINES
define|#
directive|define
name|XDL_GUESS_NLINES
value|256
end_define
begin_function
DECL|function|xdl_emit_diffrec
name|int
name|xdl_emit_diffrec
parameter_list|(
name|char
specifier|const
modifier|*
name|rec
parameter_list|,
name|long
name|size
parameter_list|,
name|char
specifier|const
modifier|*
name|pre
parameter_list|,
name|long
name|psize
parameter_list|,
name|xdemitcb_t
modifier|*
name|ecb
parameter_list|)
block|{
name|mmbuffer_t
name|mb
index|[
literal|3
index|]
decl_stmt|;
name|int
name|i
decl_stmt|;
name|mb
index|[
literal|0
index|]
operator|.
name|ptr
operator|=
operator|(
name|char
operator|*
operator|)
name|pre
expr_stmt|;
name|mb
index|[
literal|0
index|]
operator|.
name|size
operator|=
name|psize
expr_stmt|;
name|mb
index|[
literal|1
index|]
operator|.
name|ptr
operator|=
operator|(
name|char
operator|*
operator|)
name|rec
expr_stmt|;
name|mb
index|[
literal|1
index|]
operator|.
name|size
operator|=
name|size
expr_stmt|;
name|i
operator|=
literal|2
expr_stmt|;
if|if
condition|(
operator|!
name|size
operator|||
name|rec
index|[
name|size
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
block|{
name|mb
index|[
literal|2
index|]
operator|.
name|ptr
operator|=
literal|"\n\\ No newline at end of file\n"
expr_stmt|;
name|mb
index|[
literal|2
index|]
operator|.
name|size
operator|=
name|strlen
argument_list|(
name|mb
index|[
literal|2
index|]
operator|.
name|ptr
argument_list|)
expr_stmt|;
name|i
operator|=
literal|3
expr_stmt|;
block|}
if|if
condition|(
name|ecb
operator|->
name|outf
argument_list|(
name|ecb
operator|->
name|priv
argument_list|,
name|mb
argument_list|,
name|i
argument_list|)
operator|<
literal|0
condition|)
block|{
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
begin_function
DECL|function|xdl_mmfile_first
name|void
modifier|*
name|xdl_mmfile_first
parameter_list|(
name|mmfile_t
modifier|*
name|mmf
parameter_list|,
name|long
modifier|*
name|size
parameter_list|)
block|{
operator|*
name|size
operator|=
name|mmf
operator|->
name|size
expr_stmt|;
return|return
name|mmf
operator|->
name|ptr
return|;
block|}
end_function
begin_function
DECL|function|xdl_mmfile_next
name|void
modifier|*
name|xdl_mmfile_next
parameter_list|(
name|mmfile_t
modifier|*
name|mmf
parameter_list|,
name|long
modifier|*
name|size
parameter_list|)
block|{
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|xdl_mmfile_size
name|long
name|xdl_mmfile_size
parameter_list|(
name|mmfile_t
modifier|*
name|mmf
parameter_list|)
block|{
return|return
name|mmf
operator|->
name|size
return|;
block|}
end_function
begin_function
DECL|function|xdl_cha_init
name|int
name|xdl_cha_init
parameter_list|(
name|chastore_t
modifier|*
name|cha
parameter_list|,
name|long
name|isize
parameter_list|,
name|long
name|icount
parameter_list|)
block|{
name|cha
operator|->
name|head
operator|=
name|cha
operator|->
name|tail
operator|=
name|NULL
expr_stmt|;
name|cha
operator|->
name|isize
operator|=
name|isize
expr_stmt|;
name|cha
operator|->
name|nsize
operator|=
name|icount
operator|*
name|isize
expr_stmt|;
name|cha
operator|->
name|ancur
operator|=
name|cha
operator|->
name|sncur
operator|=
name|NULL
expr_stmt|;
name|cha
operator|->
name|scurr
operator|=
literal|0
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|xdl_cha_free
name|void
name|xdl_cha_free
parameter_list|(
name|chastore_t
modifier|*
name|cha
parameter_list|)
block|{
name|chanode_t
modifier|*
name|cur
decl_stmt|,
modifier|*
name|tmp
decl_stmt|;
for|for
control|(
name|cur
operator|=
name|cha
operator|->
name|head
init|;
operator|(
name|tmp
operator|=
name|cur
operator|)
operator|!=
name|NULL
condition|;
control|)
block|{
name|cur
operator|=
name|cur
operator|->
name|next
expr_stmt|;
name|xdl_free
argument_list|(
name|tmp
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|xdl_cha_alloc
name|void
modifier|*
name|xdl_cha_alloc
parameter_list|(
name|chastore_t
modifier|*
name|cha
parameter_list|)
block|{
name|chanode_t
modifier|*
name|ancur
decl_stmt|;
name|void
modifier|*
name|data
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|ancur
operator|=
name|cha
operator|->
name|ancur
operator|)
operator|||
name|ancur
operator|->
name|icurr
operator|==
name|cha
operator|->
name|nsize
condition|)
block|{
if|if
condition|(
operator|!
operator|(
name|ancur
operator|=
operator|(
name|chanode_t
operator|*
operator|)
name|xdl_malloc
argument_list|(
sizeof|sizeof
argument_list|(
name|chanode_t
argument_list|)
operator|+
name|cha
operator|->
name|nsize
argument_list|)
operator|)
condition|)
block|{
return|return
name|NULL
return|;
block|}
name|ancur
operator|->
name|icurr
operator|=
literal|0
expr_stmt|;
name|ancur
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|cha
operator|->
name|tail
condition|)
name|cha
operator|->
name|tail
operator|->
name|next
operator|=
name|ancur
expr_stmt|;
if|if
condition|(
operator|!
name|cha
operator|->
name|head
condition|)
name|cha
operator|->
name|head
operator|=
name|ancur
expr_stmt|;
name|cha
operator|->
name|tail
operator|=
name|ancur
expr_stmt|;
name|cha
operator|->
name|ancur
operator|=
name|ancur
expr_stmt|;
block|}
name|data
operator|=
operator|(
name|char
operator|*
operator|)
name|ancur
operator|+
sizeof|sizeof
argument_list|(
name|chanode_t
argument_list|)
operator|+
name|ancur
operator|->
name|icurr
expr_stmt|;
name|ancur
operator|->
name|icurr
operator|+=
name|cha
operator|->
name|isize
expr_stmt|;
return|return
name|data
return|;
block|}
end_function
begin_function
DECL|function|xdl_cha_first
name|void
modifier|*
name|xdl_cha_first
parameter_list|(
name|chastore_t
modifier|*
name|cha
parameter_list|)
block|{
name|chanode_t
modifier|*
name|sncur
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|cha
operator|->
name|sncur
operator|=
name|sncur
operator|=
name|cha
operator|->
name|head
operator|)
condition|)
return|return
name|NULL
return|;
name|cha
operator|->
name|scurr
operator|=
literal|0
expr_stmt|;
return|return
operator|(
name|char
operator|*
operator|)
name|sncur
operator|+
sizeof|sizeof
argument_list|(
name|chanode_t
argument_list|)
operator|+
name|cha
operator|->
name|scurr
return|;
block|}
end_function
begin_function
DECL|function|xdl_cha_next
name|void
modifier|*
name|xdl_cha_next
parameter_list|(
name|chastore_t
modifier|*
name|cha
parameter_list|)
block|{
name|chanode_t
modifier|*
name|sncur
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|sncur
operator|=
name|cha
operator|->
name|sncur
operator|)
condition|)
return|return
name|NULL
return|;
name|cha
operator|->
name|scurr
operator|+=
name|cha
operator|->
name|isize
expr_stmt|;
if|if
condition|(
name|cha
operator|->
name|scurr
operator|==
name|sncur
operator|->
name|icurr
condition|)
block|{
if|if
condition|(
operator|!
operator|(
name|sncur
operator|=
name|cha
operator|->
name|sncur
operator|=
name|sncur
operator|->
name|next
operator|)
condition|)
return|return
name|NULL
return|;
name|cha
operator|->
name|scurr
operator|=
literal|0
expr_stmt|;
block|}
return|return
operator|(
name|char
operator|*
operator|)
name|sncur
operator|+
sizeof|sizeof
argument_list|(
name|chanode_t
argument_list|)
operator|+
name|cha
operator|->
name|scurr
return|;
block|}
end_function
begin_function
DECL|function|xdl_guess_lines
name|long
name|xdl_guess_lines
parameter_list|(
name|mmfile_t
modifier|*
name|mf
parameter_list|)
block|{
name|long
name|nl
init|=
literal|0
decl_stmt|,
name|size
decl_stmt|,
name|tsize
init|=
literal|0
decl_stmt|;
name|char
specifier|const
modifier|*
name|data
decl_stmt|,
modifier|*
name|cur
decl_stmt|,
modifier|*
name|top
decl_stmt|;
if|if
condition|(
operator|(
name|cur
operator|=
name|data
operator|=
name|xdl_mmfile_first
argument_list|(
name|mf
argument_list|,
operator|&
name|size
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
for|for
control|(
name|top
operator|=
name|data
operator|+
name|size
init|;
name|nl
operator|<
name|XDL_GUESS_NLINES
condition|;
control|)
block|{
if|if
condition|(
name|cur
operator|>=
name|top
condition|)
block|{
name|tsize
operator|+=
call|(
name|long
call|)
argument_list|(
name|cur
operator|-
name|data
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|cur
operator|=
name|data
operator|=
name|xdl_mmfile_next
argument_list|(
name|mf
argument_list|,
operator|&
name|size
argument_list|)
operator|)
condition|)
break|break;
name|top
operator|=
name|data
operator|+
name|size
expr_stmt|;
block|}
name|nl
operator|++
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|cur
operator|=
name|memchr
argument_list|(
name|cur
argument_list|,
literal|'\n'
argument_list|,
name|top
operator|-
name|cur
argument_list|)
operator|)
condition|)
name|cur
operator|=
name|top
expr_stmt|;
else|else
name|cur
operator|++
expr_stmt|;
block|}
name|tsize
operator|+=
call|(
name|long
call|)
argument_list|(
name|cur
operator|-
name|data
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|nl
operator|&&
name|tsize
condition|)
name|nl
operator|=
name|xdl_mmfile_size
argument_list|(
name|mf
argument_list|)
operator|/
operator|(
name|tsize
operator|/
name|nl
operator|)
expr_stmt|;
return|return
name|nl
operator|+
literal|1
return|;
block|}
end_function
begin_function
DECL|function|xdl_hash_record
name|unsigned
name|long
name|xdl_hash_record
parameter_list|(
name|char
specifier|const
modifier|*
modifier|*
name|data
parameter_list|,
name|char
specifier|const
modifier|*
name|top
parameter_list|)
block|{
name|unsigned
name|long
name|ha
init|=
literal|5381
decl_stmt|;
name|char
specifier|const
modifier|*
name|ptr
init|=
operator|*
name|data
decl_stmt|;
for|for
control|(
init|;
name|ptr
operator|<
name|top
operator|&&
operator|*
name|ptr
operator|!=
literal|'\n'
condition|;
name|ptr
operator|++
control|)
block|{
name|ha
operator|+=
operator|(
name|ha
operator|<<
literal|5
operator|)
expr_stmt|;
name|ha
operator|^=
operator|(
name|unsigned
name|long
operator|)
operator|*
name|ptr
expr_stmt|;
block|}
operator|*
name|data
operator|=
name|ptr
operator|<
name|top
condition|?
name|ptr
operator|+
literal|1
else|:
name|ptr
expr_stmt|;
return|return
name|ha
return|;
block|}
end_function
begin_function
DECL|function|xdl_hashbits
name|unsigned
name|int
name|xdl_hashbits
parameter_list|(
name|unsigned
name|int
name|size
parameter_list|)
block|{
name|unsigned
name|int
name|val
init|=
literal|1
decl_stmt|,
name|bits
init|=
literal|0
decl_stmt|;
for|for
control|(
init|;
name|val
operator|<
name|size
operator|&&
name|bits
operator|<
name|CHAR_BIT
operator|*
expr|sizeof
operator|(
name|unsigned
name|int
operator|)
condition|;
name|val
operator|<<=
literal|1
operator|,
name|bits
operator|++
control|)
empty_stmt|;
return|return
name|bits
condition|?
name|bits
else|:
literal|1
return|;
block|}
end_function
begin_function
DECL|function|xdl_num_out
name|int
name|xdl_num_out
parameter_list|(
name|char
modifier|*
name|out
parameter_list|,
name|long
name|val
parameter_list|)
block|{
name|char
modifier|*
name|ptr
decl_stmt|,
modifier|*
name|str
init|=
name|out
decl_stmt|;
name|char
name|buf
index|[
literal|32
index|]
decl_stmt|;
name|ptr
operator|=
name|buf
operator|+
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
operator|-
literal|1
expr_stmt|;
operator|*
name|ptr
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
name|val
operator|<
literal|0
condition|)
block|{
operator|*
operator|--
name|ptr
operator|=
literal|'-'
expr_stmt|;
name|val
operator|=
operator|-
name|val
expr_stmt|;
block|}
for|for
control|(
init|;
name|val
operator|&&
name|ptr
operator|>
name|buf
condition|;
name|val
operator|/=
literal|10
control|)
operator|*
operator|--
name|ptr
operator|=
literal|"0123456789"
index|[
name|val
operator|%
literal|10
index|]
expr_stmt|;
if|if
condition|(
operator|*
name|ptr
condition|)
for|for
control|(
init|;
operator|*
name|ptr
condition|;
name|ptr
operator|++
operator|,
name|str
operator|++
control|)
operator|*
name|str
operator|=
operator|*
name|ptr
expr_stmt|;
else|else
operator|*
name|str
operator|++
operator|=
literal|'0'
expr_stmt|;
operator|*
name|str
operator|=
literal|'\0'
expr_stmt|;
return|return
name|str
operator|-
name|out
return|;
block|}
end_function
begin_function
DECL|function|xdl_atol
name|long
name|xdl_atol
parameter_list|(
name|char
specifier|const
modifier|*
name|str
parameter_list|,
name|char
specifier|const
modifier|*
modifier|*
name|next
parameter_list|)
block|{
name|long
name|val
decl_stmt|,
name|base
decl_stmt|;
name|char
specifier|const
modifier|*
name|top
decl_stmt|;
for|for
control|(
name|top
operator|=
name|str
init|;
name|XDL_ISDIGIT
argument_list|(
operator|*
name|top
argument_list|)
condition|;
name|top
operator|++
control|)
empty_stmt|;
if|if
condition|(
name|next
condition|)
operator|*
name|next
operator|=
name|top
expr_stmt|;
for|for
control|(
name|val
operator|=
literal|0
operator|,
name|base
operator|=
literal|1
operator|,
name|top
operator|--
init|;
name|top
operator|>=
name|str
condition|;
name|top
operator|--
operator|,
name|base
operator|*=
literal|10
control|)
name|val
operator|+=
name|base
operator|*
call|(
name|long
call|)
argument_list|(
operator|*
name|top
operator|-
literal|'0'
argument_list|)
expr_stmt|;
return|return
name|val
return|;
block|}
end_function
begin_function
DECL|function|xdl_emit_hunk_hdr
name|int
name|xdl_emit_hunk_hdr
parameter_list|(
name|long
name|s1
parameter_list|,
name|long
name|c1
parameter_list|,
name|long
name|s2
parameter_list|,
name|long
name|c2
parameter_list|,
name|xdemitcb_t
modifier|*
name|ecb
parameter_list|)
block|{
name|int
name|nb
init|=
literal|0
decl_stmt|;
name|mmbuffer_t
name|mb
decl_stmt|;
name|char
name|buf
index|[
literal|128
index|]
decl_stmt|;
name|memcpy
argument_list|(
name|buf
argument_list|,
literal|"@@ -"
argument_list|,
literal|4
argument_list|)
expr_stmt|;
name|nb
operator|+=
literal|4
expr_stmt|;
name|nb
operator|+=
name|xdl_num_out
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
name|c1
condition|?
name|s1
else|:
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|c1
operator|!=
literal|1
condition|)
block|{
name|memcpy
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
literal|","
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|nb
operator|+=
literal|1
expr_stmt|;
name|nb
operator|+=
name|xdl_num_out
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
name|c1
argument_list|)
expr_stmt|;
block|}
name|memcpy
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
literal|" +"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|nb
operator|+=
literal|2
expr_stmt|;
name|nb
operator|+=
name|xdl_num_out
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
name|c2
condition|?
name|s2
else|:
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|c2
operator|!=
literal|1
condition|)
block|{
name|memcpy
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
literal|","
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|nb
operator|+=
literal|1
expr_stmt|;
name|nb
operator|+=
name|xdl_num_out
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
name|c2
argument_list|)
expr_stmt|;
block|}
name|memcpy
argument_list|(
name|buf
operator|+
name|nb
argument_list|,
literal|" @@\n"
argument_list|,
literal|4
argument_list|)
expr_stmt|;
name|nb
operator|+=
literal|4
expr_stmt|;
name|mb
operator|.
name|ptr
operator|=
name|buf
expr_stmt|;
name|mb
operator|.
name|size
operator|=
name|nb
expr_stmt|;
if|if
condition|(
name|ecb
operator|->
name|outf
argument_list|(
name|ecb
operator|->
name|priv
argument_list|,
operator|&
name|mb
argument_list|,
literal|1
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
end_unit
