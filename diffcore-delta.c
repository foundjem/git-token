begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"diffcore.h"
end_include
begin_comment
comment|/*  * Idea here is very simple.  *  * Almost all data we are interested in are text, but sometimes we have  * to deal with binary data.  So we cut them into chunks delimited by  * LF byte, or 64-byte sequence, whichever comes first, and hash them.  *  * For those chunks, if the source buffer has more instances of it  * than the destination buffer, that means the difference are the  * number of bytes not copied from source to destination.  If the  * counts are the same, everything was copied from source to  * destination.  If the destination has more, everything was copied,  * and destination added more.  *  * We are doing an approximation so we do not really have to waste  * memory by actually storing the sequence.  We just hash them into  * somewhere around 2^16 hashbuckets and count the occurrences.  */
end_comment
begin_comment
comment|/* Wild guess at the initial hash size */
end_comment
begin_define
DECL|macro|INITIAL_HASH_SIZE
define|#
directive|define
name|INITIAL_HASH_SIZE
value|9
end_define
begin_comment
comment|/* We leave more room in smaller hash but do not let it  * grow to have unused hole too much.  */
end_comment
begin_define
DECL|macro|INITIAL_FREE
define|#
directive|define
name|INITIAL_FREE
parameter_list|(
name|sz_log2
parameter_list|)
value|((1<<(sz_log2))*(sz_log2-3)/(sz_log2))
end_define
begin_comment
comment|/* A prime rather carefully chosen between 2^16..2^17, so that  * HASHBASE< INITIAL_FREE(17).  We want to keep the maximum hashtable  * size under the current 2<<17 maximum, which can hold this many  * different values before overflowing to hashtable of size 2<<18.  */
end_comment
begin_define
DECL|macro|HASHBASE
define|#
directive|define
name|HASHBASE
value|107927
end_define
begin_struct
DECL|struct|spanhash
struct|struct
name|spanhash
block|{
DECL|member|hashval
name|unsigned
name|int
name|hashval
decl_stmt|;
DECL|member|cnt
name|unsigned
name|int
name|cnt
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|spanhash_top
struct|struct
name|spanhash_top
block|{
DECL|member|alloc_log2
name|int
name|alloc_log2
decl_stmt|;
DECL|member|free
name|int
name|free
decl_stmt|;
DECL|member|data
name|struct
name|spanhash
name|data
index|[
name|FLEX_ARRAY
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|spanhash_rehash
specifier|static
name|struct
name|spanhash_top
modifier|*
name|spanhash_rehash
parameter_list|(
name|struct
name|spanhash_top
modifier|*
name|orig
parameter_list|)
block|{
name|struct
name|spanhash_top
modifier|*
name|new
decl_stmt|;
name|int
name|i
decl_stmt|;
name|int
name|osz
init|=
literal|1
operator|<<
name|orig
operator|->
name|alloc_log2
decl_stmt|;
name|int
name|sz
init|=
name|osz
operator|<<
literal|1
decl_stmt|;
name|new
operator|=
name|xmalloc
argument_list|(
name|st_add
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|orig
argument_list|)
argument_list|,
name|st_mult
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|spanhash
argument_list|)
argument_list|,
name|sz
argument_list|)
argument_list|)
argument_list|)
expr_stmt|;
name|new
operator|->
name|alloc_log2
operator|=
name|orig
operator|->
name|alloc_log2
operator|+
literal|1
expr_stmt|;
name|new
operator|->
name|free
operator|=
name|INITIAL_FREE
argument_list|(
name|new
operator|->
name|alloc_log2
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|new
operator|->
name|data
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|spanhash
argument_list|)
operator|*
name|sz
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
name|osz
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|spanhash
modifier|*
name|o
init|=
operator|&
operator|(
name|orig
operator|->
name|data
index|[
name|i
index|]
operator|)
decl_stmt|;
name|int
name|bucket
decl_stmt|;
if|if
condition|(
operator|!
name|o
operator|->
name|cnt
condition|)
continue|continue;
name|bucket
operator|=
name|o
operator|->
name|hashval
operator|&
operator|(
name|sz
operator|-
literal|1
operator|)
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|struct
name|spanhash
modifier|*
name|h
init|=
operator|&
operator|(
name|new
operator|->
name|data
index|[
name|bucket
operator|++
index|]
operator|)
decl_stmt|;
if|if
condition|(
operator|!
name|h
operator|->
name|cnt
condition|)
block|{
name|h
operator|->
name|hashval
operator|=
name|o
operator|->
name|hashval
expr_stmt|;
name|h
operator|->
name|cnt
operator|=
name|o
operator|->
name|cnt
expr_stmt|;
name|new
operator|->
name|free
operator|--
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|sz
operator|<=
name|bucket
condition|)
name|bucket
operator|=
literal|0
expr_stmt|;
block|}
block|}
name|free
argument_list|(
name|orig
argument_list|)
expr_stmt|;
return|return
name|new
return|;
block|}
end_function
begin_function
DECL|function|add_spanhash
specifier|static
name|struct
name|spanhash_top
modifier|*
name|add_spanhash
parameter_list|(
name|struct
name|spanhash_top
modifier|*
name|top
parameter_list|,
name|unsigned
name|int
name|hashval
parameter_list|,
name|int
name|cnt
parameter_list|)
block|{
name|int
name|bucket
decl_stmt|,
name|lim
decl_stmt|;
name|struct
name|spanhash
modifier|*
name|h
decl_stmt|;
name|lim
operator|=
operator|(
literal|1
operator|<<
name|top
operator|->
name|alloc_log2
operator|)
expr_stmt|;
name|bucket
operator|=
name|hashval
operator|&
operator|(
name|lim
operator|-
literal|1
operator|)
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|h
operator|=
operator|&
operator|(
name|top
operator|->
name|data
index|[
name|bucket
operator|++
index|]
operator|)
expr_stmt|;
if|if
condition|(
operator|!
name|h
operator|->
name|cnt
condition|)
block|{
name|h
operator|->
name|hashval
operator|=
name|hashval
expr_stmt|;
name|h
operator|->
name|cnt
operator|=
name|cnt
expr_stmt|;
name|top
operator|->
name|free
operator|--
expr_stmt|;
if|if
condition|(
name|top
operator|->
name|free
operator|<
literal|0
condition|)
return|return
name|spanhash_rehash
argument_list|(
name|top
argument_list|)
return|;
return|return
name|top
return|;
block|}
if|if
condition|(
name|h
operator|->
name|hashval
operator|==
name|hashval
condition|)
block|{
name|h
operator|->
name|cnt
operator|+=
name|cnt
expr_stmt|;
return|return
name|top
return|;
block|}
if|if
condition|(
name|lim
operator|<=
name|bucket
condition|)
name|bucket
operator|=
literal|0
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|spanhash_cmp
specifier|static
name|int
name|spanhash_cmp
parameter_list|(
specifier|const
name|void
modifier|*
name|a_
parameter_list|,
specifier|const
name|void
modifier|*
name|b_
parameter_list|)
block|{
specifier|const
name|struct
name|spanhash
modifier|*
name|a
init|=
name|a_
decl_stmt|;
specifier|const
name|struct
name|spanhash
modifier|*
name|b
init|=
name|b_
decl_stmt|;
comment|/* A count of zero compares at the end.. */
if|if
condition|(
operator|!
name|a
operator|->
name|cnt
condition|)
return|return
operator|!
name|b
operator|->
name|cnt
condition|?
literal|0
else|:
literal|1
return|;
if|if
condition|(
operator|!
name|b
operator|->
name|cnt
condition|)
return|return
operator|-
literal|1
return|;
return|return
name|a
operator|->
name|hashval
operator|<
name|b
operator|->
name|hashval
condition|?
operator|-
literal|1
else|:
name|a
operator|->
name|hashval
operator|>
name|b
operator|->
name|hashval
condition|?
literal|1
else|:
literal|0
return|;
block|}
end_function
begin_function
DECL|function|hash_chars
specifier|static
name|struct
name|spanhash_top
modifier|*
name|hash_chars
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|one
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|n
decl_stmt|;
name|unsigned
name|int
name|accum1
decl_stmt|,
name|accum2
decl_stmt|,
name|hashval
decl_stmt|;
name|struct
name|spanhash_top
modifier|*
name|hash
decl_stmt|;
name|unsigned
name|char
modifier|*
name|buf
init|=
name|one
operator|->
name|data
decl_stmt|;
name|unsigned
name|int
name|sz
init|=
name|one
operator|->
name|size
decl_stmt|;
name|int
name|is_text
init|=
operator|!
name|diff_filespec_is_binary
argument_list|(
name|one
argument_list|)
decl_stmt|;
name|i
operator|=
name|INITIAL_HASH_SIZE
expr_stmt|;
name|hash
operator|=
name|xmalloc
argument_list|(
name|st_add
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|hash
argument_list|)
argument_list|,
name|st_mult
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|spanhash
argument_list|)
argument_list|,
literal|1
operator|<<
name|i
argument_list|)
argument_list|)
argument_list|)
expr_stmt|;
name|hash
operator|->
name|alloc_log2
operator|=
name|i
expr_stmt|;
name|hash
operator|->
name|free
operator|=
name|INITIAL_FREE
argument_list|(
name|i
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|hash
operator|->
name|data
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|spanhash
argument_list|)
operator|*
operator|(
literal|1
operator|<<
name|i
operator|)
argument_list|)
expr_stmt|;
name|n
operator|=
literal|0
expr_stmt|;
name|accum1
operator|=
name|accum2
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|sz
condition|)
block|{
name|unsigned
name|int
name|c
init|=
operator|*
name|buf
operator|++
decl_stmt|;
name|unsigned
name|int
name|old_1
init|=
name|accum1
decl_stmt|;
name|sz
operator|--
expr_stmt|;
comment|/* Ignore CR in CRLF sequence if text */
if|if
condition|(
name|is_text
operator|&&
name|c
operator|==
literal|'\r'
operator|&&
name|sz
operator|&&
operator|*
name|buf
operator|==
literal|'\n'
condition|)
continue|continue;
name|accum1
operator|=
operator|(
name|accum1
operator|<<
literal|7
operator|)
operator|^
operator|(
name|accum2
operator|>>
literal|25
operator|)
expr_stmt|;
name|accum2
operator|=
operator|(
name|accum2
operator|<<
literal|7
operator|)
operator|^
operator|(
name|old_1
operator|>>
literal|25
operator|)
expr_stmt|;
name|accum1
operator|+=
name|c
expr_stmt|;
if|if
condition|(
operator|++
name|n
operator|<
literal|64
operator|&&
name|c
operator|!=
literal|'\n'
condition|)
continue|continue;
name|hashval
operator|=
operator|(
name|accum1
operator|+
name|accum2
operator|*
literal|0x61
operator|)
operator|%
name|HASHBASE
expr_stmt|;
name|hash
operator|=
name|add_spanhash
argument_list|(
name|hash
argument_list|,
name|hashval
argument_list|,
name|n
argument_list|)
expr_stmt|;
name|n
operator|=
literal|0
expr_stmt|;
name|accum1
operator|=
name|accum2
operator|=
literal|0
expr_stmt|;
block|}
name|qsort
argument_list|(
name|hash
operator|->
name|data
argument_list|,
literal|1ul
operator|<<
name|hash
operator|->
name|alloc_log2
argument_list|,
sizeof|sizeof
argument_list|(
name|hash
operator|->
name|data
index|[
literal|0
index|]
argument_list|)
argument_list|,
name|spanhash_cmp
argument_list|)
expr_stmt|;
return|return
name|hash
return|;
block|}
end_function
begin_function
DECL|function|diffcore_count_changes
name|int
name|diffcore_count_changes
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|src
parameter_list|,
name|struct
name|diff_filespec
modifier|*
name|dst
parameter_list|,
name|void
modifier|*
modifier|*
name|src_count_p
parameter_list|,
name|void
modifier|*
modifier|*
name|dst_count_p
parameter_list|,
name|unsigned
name|long
name|delta_limit
parameter_list|,
name|unsigned
name|long
modifier|*
name|src_copied
parameter_list|,
name|unsigned
name|long
modifier|*
name|literal_added
parameter_list|)
block|{
name|struct
name|spanhash
modifier|*
name|s
decl_stmt|,
modifier|*
name|d
decl_stmt|;
name|struct
name|spanhash_top
modifier|*
name|src_count
decl_stmt|,
modifier|*
name|dst_count
decl_stmt|;
name|unsigned
name|long
name|sc
decl_stmt|,
name|la
decl_stmt|;
name|src_count
operator|=
name|dst_count
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|src_count_p
condition|)
name|src_count
operator|=
operator|*
name|src_count_p
expr_stmt|;
if|if
condition|(
operator|!
name|src_count
condition|)
block|{
name|src_count
operator|=
name|hash_chars
argument_list|(
name|src
argument_list|)
expr_stmt|;
if|if
condition|(
name|src_count_p
condition|)
operator|*
name|src_count_p
operator|=
name|src_count
expr_stmt|;
block|}
if|if
condition|(
name|dst_count_p
condition|)
name|dst_count
operator|=
operator|*
name|dst_count_p
expr_stmt|;
if|if
condition|(
operator|!
name|dst_count
condition|)
block|{
name|dst_count
operator|=
name|hash_chars
argument_list|(
name|dst
argument_list|)
expr_stmt|;
if|if
condition|(
name|dst_count_p
condition|)
operator|*
name|dst_count_p
operator|=
name|dst_count
expr_stmt|;
block|}
name|sc
operator|=
name|la
operator|=
literal|0
expr_stmt|;
name|s
operator|=
name|src_count
operator|->
name|data
expr_stmt|;
name|d
operator|=
name|dst_count
operator|->
name|data
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|unsigned
name|dst_cnt
decl_stmt|,
name|src_cnt
decl_stmt|;
if|if
condition|(
operator|!
name|s
operator|->
name|cnt
condition|)
break|break;
comment|/* we checked all in src */
while|while
condition|(
name|d
operator|->
name|cnt
condition|)
block|{
if|if
condition|(
name|d
operator|->
name|hashval
operator|>=
name|s
operator|->
name|hashval
condition|)
break|break;
name|la
operator|+=
name|d
operator|->
name|cnt
expr_stmt|;
name|d
operator|++
expr_stmt|;
block|}
name|src_cnt
operator|=
name|s
operator|->
name|cnt
expr_stmt|;
name|dst_cnt
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|d
operator|->
name|cnt
operator|&&
name|d
operator|->
name|hashval
operator|==
name|s
operator|->
name|hashval
condition|)
block|{
name|dst_cnt
operator|=
name|d
operator|->
name|cnt
expr_stmt|;
name|d
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|src_cnt
operator|<
name|dst_cnt
condition|)
block|{
name|la
operator|+=
name|dst_cnt
operator|-
name|src_cnt
expr_stmt|;
name|sc
operator|+=
name|src_cnt
expr_stmt|;
block|}
else|else
name|sc
operator|+=
name|dst_cnt
expr_stmt|;
name|s
operator|++
expr_stmt|;
block|}
while|while
condition|(
name|d
operator|->
name|cnt
condition|)
block|{
name|la
operator|+=
name|d
operator|->
name|cnt
expr_stmt|;
name|d
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|src_count_p
condition|)
name|free
argument_list|(
name|src_count
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|dst_count_p
condition|)
name|free
argument_list|(
name|dst_count
argument_list|)
expr_stmt|;
operator|*
name|src_copied
operator|=
name|sc
expr_stmt|;
operator|*
name|literal_added
operator|=
name|la
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
