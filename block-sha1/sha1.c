begin_unit
begin_comment
comment|/*  * Based on the Mozilla SHA1 (see mozilla-sha1/sha1.c),  * optimized to do word accesses rather than byte accesses,  * and to avoid unnecessary copies into the context array.  */
end_comment
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<arpa/inet.h>
end_include
begin_include
include|#
directive|include
file|"sha1.h"
end_include
begin_comment
comment|/* Hash one 64-byte block of data */
end_comment
begin_function_decl
specifier|static
name|void
name|blk_SHA1Block
parameter_list|(
name|blk_SHA_CTX
modifier|*
name|ctx
parameter_list|,
specifier|const
name|unsigned
name|int
modifier|*
name|data
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|blk_SHA1_Init
name|void
name|blk_SHA1_Init
parameter_list|(
name|blk_SHA_CTX
modifier|*
name|ctx
parameter_list|)
block|{
name|ctx
operator|->
name|lenW
operator|=
literal|0
expr_stmt|;
name|ctx
operator|->
name|size
operator|=
literal|0
expr_stmt|;
comment|/* Initialize H with the magic constants (see FIPS180 for constants) 	 */
name|ctx
operator|->
name|H
index|[
literal|0
index|]
operator|=
literal|0x67452301
expr_stmt|;
name|ctx
operator|->
name|H
index|[
literal|1
index|]
operator|=
literal|0xefcdab89
expr_stmt|;
name|ctx
operator|->
name|H
index|[
literal|2
index|]
operator|=
literal|0x98badcfe
expr_stmt|;
name|ctx
operator|->
name|H
index|[
literal|3
index|]
operator|=
literal|0x10325476
expr_stmt|;
name|ctx
operator|->
name|H
index|[
literal|4
index|]
operator|=
literal|0xc3d2e1f0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|blk_SHA1_Update
name|void
name|blk_SHA1_Update
parameter_list|(
name|blk_SHA_CTX
modifier|*
name|ctx
parameter_list|,
specifier|const
name|void
modifier|*
name|data
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|)
block|{
name|int
name|lenW
init|=
name|ctx
operator|->
name|lenW
decl_stmt|;
name|ctx
operator|->
name|size
operator|+=
operator|(
name|unsigned
name|long
name|long
operator|)
name|len
operator|<<
literal|3
expr_stmt|;
comment|/* Read the data into W and process blocks as they get full 	 */
if|if
condition|(
name|lenW
condition|)
block|{
name|int
name|left
init|=
literal|64
operator|-
name|lenW
decl_stmt|;
if|if
condition|(
name|len
operator|<
name|left
condition|)
name|left
operator|=
name|len
expr_stmt|;
name|memcpy
argument_list|(
name|lenW
operator|+
operator|(
name|char
operator|*
operator|)
name|ctx
operator|->
name|W
argument_list|,
name|data
argument_list|,
name|left
argument_list|)
expr_stmt|;
name|lenW
operator|=
operator|(
name|lenW
operator|+
name|left
operator|)
operator|&
literal|63
expr_stmt|;
name|len
operator|-=
name|left
expr_stmt|;
name|data
operator|+=
name|left
expr_stmt|;
name|ctx
operator|->
name|lenW
operator|=
name|lenW
expr_stmt|;
if|if
condition|(
name|lenW
condition|)
return|return;
name|blk_SHA1Block
argument_list|(
name|ctx
argument_list|,
name|ctx
operator|->
name|W
argument_list|)
expr_stmt|;
block|}
while|while
condition|(
name|len
operator|>=
literal|64
condition|)
block|{
name|blk_SHA1Block
argument_list|(
name|ctx
argument_list|,
name|data
argument_list|)
expr_stmt|;
name|data
operator|+=
literal|64
expr_stmt|;
name|len
operator|-=
literal|64
expr_stmt|;
block|}
if|if
condition|(
name|len
condition|)
block|{
name|memcpy
argument_list|(
name|ctx
operator|->
name|W
argument_list|,
name|data
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|ctx
operator|->
name|lenW
operator|=
name|len
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|blk_SHA1_Final
name|void
name|blk_SHA1_Final
parameter_list|(
name|unsigned
name|char
name|hashout
index|[
literal|20
index|]
parameter_list|,
name|blk_SHA_CTX
modifier|*
name|ctx
parameter_list|)
block|{
specifier|static
specifier|const
name|unsigned
name|char
name|pad
index|[
literal|64
index|]
init|=
block|{
literal|0x80
block|}
decl_stmt|;
name|unsigned
name|int
name|padlen
index|[
literal|2
index|]
decl_stmt|;
name|int
name|i
decl_stmt|;
comment|/* Pad with a binary 1 (ie 0x80), then zeroes, then length 	 */
name|padlen
index|[
literal|0
index|]
operator|=
name|htonl
argument_list|(
name|ctx
operator|->
name|size
operator|>>
literal|32
argument_list|)
expr_stmt|;
name|padlen
index|[
literal|1
index|]
operator|=
name|htonl
argument_list|(
name|ctx
operator|->
name|size
argument_list|)
expr_stmt|;
name|blk_SHA1_Update
argument_list|(
name|ctx
argument_list|,
name|pad
argument_list|,
literal|1
operator|+
operator|(
literal|63
operator|&
operator|(
literal|55
operator|-
name|ctx
operator|->
name|lenW
operator|)
operator|)
argument_list|)
expr_stmt|;
name|blk_SHA1_Update
argument_list|(
name|ctx
argument_list|,
name|padlen
argument_list|,
literal|8
argument_list|)
expr_stmt|;
comment|/* Output hash 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|5
condition|;
name|i
operator|++
control|)
operator|(
operator|(
name|unsigned
name|int
operator|*
operator|)
name|hashout
operator|)
index|[
name|i
index|]
operator|=
name|htonl
argument_list|(
name|ctx
operator|->
name|H
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
end_function
begin_define
DECL|macro|SHA_ROT
define|#
directive|define
name|SHA_ROT
parameter_list|(
name|X
parameter_list|,
name|n
parameter_list|)
value|(((X)<< (n)) | ((X)>> (32-(n))))
end_define
begin_function
DECL|function|blk_SHA1Block
specifier|static
name|void
name|blk_SHA1Block
parameter_list|(
name|blk_SHA_CTX
modifier|*
name|ctx
parameter_list|,
specifier|const
name|unsigned
name|int
modifier|*
name|data
parameter_list|)
block|{
name|int
name|t
decl_stmt|;
name|unsigned
name|int
name|A
decl_stmt|,
name|B
decl_stmt|,
name|C
decl_stmt|,
name|D
decl_stmt|,
name|E
decl_stmt|,
name|TEMP
decl_stmt|;
name|unsigned
name|int
name|W
index|[
literal|80
index|]
decl_stmt|;
for|for
control|(
name|t
operator|=
literal|0
init|;
name|t
operator|<
literal|16
condition|;
name|t
operator|++
control|)
name|W
index|[
name|t
index|]
operator|=
name|htonl
argument_list|(
name|data
index|[
name|t
index|]
argument_list|)
expr_stmt|;
comment|/* Unroll it? */
for|for
control|(
name|t
operator|=
literal|16
init|;
name|t
operator|<=
literal|79
condition|;
name|t
operator|++
control|)
name|W
index|[
name|t
index|]
operator|=
name|SHA_ROT
argument_list|(
name|W
index|[
name|t
operator|-
literal|3
index|]
operator|^
name|W
index|[
name|t
operator|-
literal|8
index|]
operator|^
name|W
index|[
name|t
operator|-
literal|14
index|]
operator|^
name|W
index|[
name|t
operator|-
literal|16
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|A
operator|=
name|ctx
operator|->
name|H
index|[
literal|0
index|]
expr_stmt|;
name|B
operator|=
name|ctx
operator|->
name|H
index|[
literal|1
index|]
expr_stmt|;
name|C
operator|=
name|ctx
operator|->
name|H
index|[
literal|2
index|]
expr_stmt|;
name|D
operator|=
name|ctx
operator|->
name|H
index|[
literal|3
index|]
expr_stmt|;
name|E
operator|=
name|ctx
operator|->
name|H
index|[
literal|4
index|]
expr_stmt|;
DECL|macro|T_0_19
define|#
directive|define
name|T_0_19
parameter_list|(
name|t
parameter_list|)
define|\
value|TEMP = SHA_ROT(A,5) + (((C^D)&B)^D)     + E + W[t] + 0x5a827999; \ 	E = D; D = C; C = SHA_ROT(B, 30); B = A; A = TEMP;
name|T_0_19
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|2
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|3
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|4
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|5
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|6
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|7
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|8
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|9
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|10
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|11
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|12
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|13
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|14
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|15
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|16
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|17
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|18
argument_list|)
expr_stmt|;
name|T_0_19
argument_list|(
literal|19
argument_list|)
expr_stmt|;
DECL|macro|T_20_39
define|#
directive|define
name|T_20_39
parameter_list|(
name|t
parameter_list|)
define|\
value|TEMP = SHA_ROT(A,5) + (B^C^D)           + E + W[t] + 0x6ed9eba1; \ 	E = D; D = C; C = SHA_ROT(B, 30); B = A; A = TEMP;
name|T_20_39
argument_list|(
literal|20
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|21
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|22
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|23
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|24
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|25
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|26
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|27
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|28
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|29
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|30
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|31
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|32
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|33
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|34
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|35
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|36
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|37
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|38
argument_list|)
expr_stmt|;
name|T_20_39
argument_list|(
literal|39
argument_list|)
expr_stmt|;
DECL|macro|T_40_59
define|#
directive|define
name|T_40_59
parameter_list|(
name|t
parameter_list|)
define|\
value|TEMP = SHA_ROT(A,5) + ((B&C)|(D&(B|C))) + E + W[t] + 0x8f1bbcdc; \ 	E = D; D = C; C = SHA_ROT(B, 30); B = A; A = TEMP;
name|T_40_59
argument_list|(
literal|40
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|41
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|42
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|43
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|44
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|45
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|46
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|47
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|48
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|49
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|50
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|51
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|52
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|53
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|54
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|55
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|56
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|57
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|58
argument_list|)
expr_stmt|;
name|T_40_59
argument_list|(
literal|59
argument_list|)
expr_stmt|;
DECL|macro|T_60_79
define|#
directive|define
name|T_60_79
parameter_list|(
name|t
parameter_list|)
define|\
value|TEMP = SHA_ROT(A,5) + (B^C^D)           + E + W[t] + 0xca62c1d6; \ 	E = D; D = C; C = SHA_ROT(B, 30); B = A; A = TEMP;
name|T_60_79
argument_list|(
literal|60
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|61
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|62
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|63
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|64
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|65
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|66
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|67
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|68
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|69
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|70
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|71
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|72
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|73
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|74
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|75
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|76
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|77
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|78
argument_list|)
expr_stmt|;
name|T_60_79
argument_list|(
literal|79
argument_list|)
expr_stmt|;
name|ctx
operator|->
name|H
index|[
literal|0
index|]
operator|+=
name|A
expr_stmt|;
name|ctx
operator|->
name|H
index|[
literal|1
index|]
operator|+=
name|B
expr_stmt|;
name|ctx
operator|->
name|H
index|[
literal|2
index|]
operator|+=
name|C
expr_stmt|;
name|ctx
operator|->
name|H
index|[
literal|3
index|]
operator|+=
name|D
expr_stmt|;
name|ctx
operator|->
name|H
index|[
literal|4
index|]
operator|+=
name|E
expr_stmt|;
block|}
end_function
end_unit
