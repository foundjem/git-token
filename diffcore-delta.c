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
comment|/*  * Idea here is very simple.  *  * We have total of (sz-N+1) N-byte overlapping sequences in buf whose  * size is sz.  If the same N-byte sequence appears in both source and  * destination, we say the byte that starts that sequence is shared  * between them (i.e. copied from source to destination).  *  * For each possible N-byte sequence, if the source buffer has more  * instances of it than the destination buffer, that means the  * difference are the number of bytes not copied from source to  * destination.  If the counts are the same, everything was copied  * from source to destination.  If the destination has more,  * everything was copied, and destination added more.  *  * We are doing an approximation so we do not really have to waste  * memory by actually storing the sequence.  We just hash them into  * somewhere around 2^16 hashbuckets and count the occurrences.  *  * The length of the sequence is arbitrarily set to 8 for now.  */
end_comment
begin_comment
comment|/* Wild guess at the initial hash size */
end_comment
begin_define
DECL|macro|INITIAL_HASH_SIZE
define|#
directive|define
name|INITIAL_HASH_SIZE
value|10
end_define
begin_define
DECL|macro|HASHBASE
define|#
directive|define
name|HASHBASE
value|65537
end_define
begin_comment
DECL|macro|HASHBASE
comment|/* next_prime(2^16) */
end_comment
begin_struct
DECL|struct|spanhash
struct|struct
name|spanhash
block|{
DECL|member|hashval
name|unsigned
name|long
name|hashval
decl_stmt|;
DECL|member|cnt
name|unsigned
name|long
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
DECL|function|spanhash_find
specifier|static
name|struct
name|spanhash
modifier|*
name|spanhash_find
parameter_list|(
name|struct
name|spanhash_top
modifier|*
name|top
parameter_list|,
name|unsigned
name|long
name|hashval
parameter_list|)
block|{
name|int
name|sz
init|=
literal|1
operator|<<
name|top
operator|->
name|alloc_log2
decl_stmt|;
name|int
name|bucket
init|=
name|hashval
operator|&
operator|(
name|sz
operator|-
literal|1
operator|)
decl_stmt|;
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
name|top
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
return|return
name|NULL
return|;
if|if
condition|(
name|h
operator|->
name|hashval
operator|==
name|hashval
condition|)
return|return
name|h
return|;
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
end_function
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
sizeof|sizeof
argument_list|(
operator|*
name|orig
argument_list|)
operator|+
sizeof|sizeof
argument_list|(
expr|struct
name|spanhash
argument_list|)
operator|*
name|sz
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
name|osz
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
name|long
name|hashval
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
literal|1
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
operator|++
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
DECL|function|hash_chars
specifier|static
name|struct
name|spanhash_top
modifier|*
name|hash_chars
parameter_list|(
name|unsigned
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|sz
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|unsigned
name|long
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
name|i
operator|=
name|INITIAL_HASH_SIZE
expr_stmt|;
name|hash
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|hash
argument_list|)
operator|+
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
operator|(
literal|1
operator|<<
name|i
operator|)
operator|/
literal|2
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
comment|/* an 8-byte shift register made of accum1 and accum2.  New 	 * bytes come at LSB of accum2, and shifted up to accum1 	 */
for|for
control|(
name|i
operator|=
name|accum1
operator|=
name|accum2
operator|=
literal|0
init|;
name|i
operator|<
literal|7
condition|;
name|i
operator|++
operator|,
name|sz
operator|--
control|)
block|{
name|accum1
operator|=
operator|(
name|accum1
operator|<<
literal|8
operator|)
operator||
operator|(
name|accum2
operator|>>
literal|24
operator|)
expr_stmt|;
name|accum2
operator|=
operator|(
name|accum2
operator|<<
literal|8
operator|)
operator||
operator|*
name|buf
operator|++
expr_stmt|;
block|}
while|while
condition|(
name|sz
condition|)
block|{
name|accum1
operator|=
operator|(
name|accum1
operator|<<
literal|8
operator|)
operator||
operator|(
name|accum2
operator|>>
literal|24
operator|)
expr_stmt|;
name|accum2
operator|=
operator|(
name|accum2
operator|<<
literal|8
operator|)
operator||
operator|*
name|buf
operator|++
expr_stmt|;
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
argument_list|)
expr_stmt|;
name|sz
operator|--
expr_stmt|;
block|}
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
name|void
modifier|*
name|src
parameter_list|,
name|unsigned
name|long
name|src_size
parameter_list|,
name|void
modifier|*
name|dst
parameter_list|,
name|unsigned
name|long
name|dst_size
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
name|int
name|i
decl_stmt|,
name|ssz
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
if|if
condition|(
name|src_size
operator|<
literal|8
operator|||
name|dst_size
operator|<
literal|8
condition|)
return|return
operator|-
literal|1
return|;
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
argument_list|,
name|src_size
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
argument_list|,
name|dst_size
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
name|ssz
operator|=
literal|1
operator|<<
name|src_count
operator|->
name|alloc_log2
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ssz
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|spanhash
modifier|*
name|s
init|=
operator|&
operator|(
name|src_count
operator|->
name|data
index|[
name|i
index|]
operator|)
decl_stmt|;
name|struct
name|spanhash
modifier|*
name|d
decl_stmt|;
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
continue|continue;
name|src_cnt
operator|=
name|s
operator|->
name|cnt
expr_stmt|;
name|d
operator|=
name|spanhash_find
argument_list|(
name|dst_count
argument_list|,
name|s
operator|->
name|hashval
argument_list|)
expr_stmt|;
name|dst_cnt
operator|=
name|d
condition|?
name|d
operator|->
name|cnt
else|:
literal|0
expr_stmt|;
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
