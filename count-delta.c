begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  * The delta-parsing part is almost straight copy of patch-delta.c  * which is (C) 2005 Nicolas Pitre<nico@cam.org>.  */
end_comment
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<limits.h>
end_include
begin_include
include|#
directive|include
file|"delta.h"
end_include
begin_include
include|#
directive|include
file|"count-delta.h"
end_include
begin_comment
comment|/*  * NOTE.  We do not _interpret_ delta fully.  As an approximation, we  * just count the number of bytes that are copied from the source, and  * the number of literal data bytes that are inserted.  *  * Number of bytes that are _not_ copied from the source is deletion,  * and number of inserted literal bytes are addition, so sum of them  * is the extent of damage.  xdelta can express an edit that copies  * data inside of the destination which originally came from the  * source.  We do not count that in the following routine, so we are  * undercounting the source material that remains in the final output  * that way.  */
end_comment
begin_function
DECL|function|count_delta
name|int
name|count_delta
parameter_list|(
name|void
modifier|*
name|delta_buf
parameter_list|,
name|unsigned
name|long
name|delta_size
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
name|unsigned
name|long
name|copied_from_source
decl_stmt|,
name|added_literal
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|data
decl_stmt|,
modifier|*
name|top
decl_stmt|;
name|unsigned
name|char
name|cmd
decl_stmt|;
name|unsigned
name|long
name|src_size
decl_stmt|,
name|dst_size
decl_stmt|,
name|out
decl_stmt|;
if|if
condition|(
name|delta_size
operator|<
name|DELTA_SIZE_MIN
condition|)
return|return
operator|-
literal|1
return|;
name|data
operator|=
name|delta_buf
expr_stmt|;
name|top
operator|=
name|delta_buf
operator|+
name|delta_size
expr_stmt|;
name|src_size
operator|=
name|get_delta_hdr_size
argument_list|(
operator|&
name|data
argument_list|)
expr_stmt|;
name|dst_size
operator|=
name|get_delta_hdr_size
argument_list|(
operator|&
name|data
argument_list|)
expr_stmt|;
name|added_literal
operator|=
name|copied_from_source
operator|=
name|out
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|data
operator|<
name|top
condition|)
block|{
name|cmd
operator|=
operator|*
name|data
operator|++
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x80
condition|)
block|{
name|unsigned
name|long
name|cp_off
init|=
literal|0
decl_stmt|,
name|cp_size
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x01
condition|)
name|cp_off
operator|=
operator|*
name|data
operator|++
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x02
condition|)
name|cp_off
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|8
operator|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x04
condition|)
name|cp_off
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|16
operator|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x08
condition|)
name|cp_off
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|24
operator|)
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x10
condition|)
name|cp_size
operator|=
operator|*
name|data
operator|++
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x20
condition|)
name|cp_size
operator||=
operator|(
operator|*
name|data
operator|++
operator|<<
literal|8
operator|)
expr_stmt|;
if|if
condition|(
name|cp_size
operator|==
literal|0
condition|)
name|cp_size
operator|=
literal|0x10000
expr_stmt|;
if|if
condition|(
name|cmd
operator|&
literal|0x40
condition|)
comment|/* copy from dst */
empty_stmt|;
else|else
name|copied_from_source
operator|+=
name|cp_size
expr_stmt|;
name|out
operator|+=
name|cp_size
expr_stmt|;
block|}
else|else
block|{
comment|/* write literal into dst */
name|added_literal
operator|+=
name|cmd
expr_stmt|;
name|out
operator|+=
name|cmd
expr_stmt|;
name|data
operator|+=
name|cmd
expr_stmt|;
block|}
block|}
comment|/* sanity check */
if|if
condition|(
name|data
operator|!=
name|top
operator|||
name|out
operator|!=
name|dst_size
condition|)
return|return
operator|-
literal|1
return|;
comment|/* delete size is what was _not_ copied from source. 	 * edit size is that and literal additions. 	 */
operator|*
name|src_copied
operator|=
name|copied_from_source
expr_stmt|;
operator|*
name|literal_added
operator|=
name|added_literal
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
