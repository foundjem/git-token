begin_unit
begin_comment
comment|/*  *  LibXDiff by Davide Libenzi ( File Differential Library )  *  Copyright (C) 2003	Davide Libenzi  *  *  This library is free software; you can redistribute it and/or  *  modify it under the terms of the GNU Lesser General Public  *  License as published by the Free Software Foundation; either  *  version 2.1 of the License, or (at your option) any later version.  *  *  This library is distributed in the hope that it will be useful,  *  but WITHOUT ANY WARRANTY; without even the implied warranty of  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *  Lesser General Public License for more details.  *  *  You should have received a copy of the GNU Lesser General Public  *  License along with this library; if not, write to the Free Software  *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  *  *  Davide Libenzi<davidel@xmailserver.org>  *  */
end_comment
begin_include
include|#
directive|include
file|"xinclude.h"
end_include
begin_function_decl
specifier|static
name|long
name|xdl_get_rec
parameter_list|(
name|xdfile_t
modifier|*
name|xdf
parameter_list|,
name|long
name|ri
parameter_list|,
name|char
specifier|const
modifier|*
modifier|*
name|rec
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|static
name|int
name|xdl_emit_record
parameter_list|(
name|xdfile_t
modifier|*
name|xdf
parameter_list|,
name|long
name|ri
parameter_list|,
name|char
specifier|const
modifier|*
name|pre
parameter_list|,
name|xdemitcb_t
modifier|*
name|ecb
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|static
name|xdchange_t
modifier|*
name|xdl_get_hunk
parameter_list|(
name|xdchange_t
modifier|*
name|xscr
parameter_list|,
name|xdemitconf_t
specifier|const
modifier|*
name|xecfg
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|xdl_get_rec
specifier|static
name|long
name|xdl_get_rec
parameter_list|(
name|xdfile_t
modifier|*
name|xdf
parameter_list|,
name|long
name|ri
parameter_list|,
name|char
specifier|const
modifier|*
modifier|*
name|rec
parameter_list|)
block|{
operator|*
name|rec
operator|=
name|xdf
operator|->
name|recs
index|[
name|ri
index|]
operator|->
name|ptr
expr_stmt|;
return|return
name|xdf
operator|->
name|recs
index|[
name|ri
index|]
operator|->
name|size
return|;
block|}
end_function
begin_function
DECL|function|xdl_emit_record
specifier|static
name|int
name|xdl_emit_record
parameter_list|(
name|xdfile_t
modifier|*
name|xdf
parameter_list|,
name|long
name|ri
parameter_list|,
name|char
specifier|const
modifier|*
name|pre
parameter_list|,
name|xdemitcb_t
modifier|*
name|ecb
parameter_list|)
block|{
name|long
name|size
decl_stmt|,
name|psize
init|=
name|strlen
argument_list|(
name|pre
argument_list|)
decl_stmt|;
name|char
specifier|const
modifier|*
name|rec
decl_stmt|;
name|size
operator|=
name|xdl_get_rec
argument_list|(
name|xdf
argument_list|,
name|ri
argument_list|,
operator|&
name|rec
argument_list|)
expr_stmt|;
if|if
condition|(
name|xdl_emit_diffrec
argument_list|(
name|rec
argument_list|,
name|size
argument_list|,
name|pre
argument_list|,
name|psize
argument_list|,
name|ecb
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
begin_comment
comment|/*  * Starting at the passed change atom, find the latest change atom to be included  * inside the differential hunk according to the specified configuration.  */
end_comment
begin_function
DECL|function|xdl_get_hunk
specifier|static
name|xdchange_t
modifier|*
name|xdl_get_hunk
parameter_list|(
name|xdchange_t
modifier|*
name|xscr
parameter_list|,
name|xdemitconf_t
specifier|const
modifier|*
name|xecfg
parameter_list|)
block|{
name|xdchange_t
modifier|*
name|xch
decl_stmt|,
modifier|*
name|xchp
decl_stmt|;
for|for
control|(
name|xchp
operator|=
name|xscr
operator|,
name|xch
operator|=
name|xscr
operator|->
name|next
init|;
name|xch
condition|;
name|xchp
operator|=
name|xch
operator|,
name|xch
operator|=
name|xch
operator|->
name|next
control|)
if|if
condition|(
name|xch
operator|->
name|i1
operator|-
operator|(
name|xchp
operator|->
name|i1
operator|+
name|xchp
operator|->
name|chg1
operator|)
operator|>
literal|2
operator|*
name|xecfg
operator|->
name|ctxlen
condition|)
break|break;
return|return
name|xchp
return|;
block|}
end_function
begin_function
DECL|function|xdl_emit_diff
name|int
name|xdl_emit_diff
parameter_list|(
name|xdfenv_t
modifier|*
name|xe
parameter_list|,
name|xdchange_t
modifier|*
name|xscr
parameter_list|,
name|xdemitcb_t
modifier|*
name|ecb
parameter_list|,
name|xdemitconf_t
specifier|const
modifier|*
name|xecfg
parameter_list|)
block|{
name|long
name|s1
decl_stmt|,
name|s2
decl_stmt|,
name|e1
decl_stmt|,
name|e2
decl_stmt|,
name|lctx
decl_stmt|;
name|xdchange_t
modifier|*
name|xch
decl_stmt|,
modifier|*
name|xche
decl_stmt|;
for|for
control|(
name|xch
operator|=
name|xche
operator|=
name|xscr
init|;
name|xch
condition|;
name|xch
operator|=
name|xche
operator|->
name|next
control|)
block|{
name|xche
operator|=
name|xdl_get_hunk
argument_list|(
name|xch
argument_list|,
name|xecfg
argument_list|)
expr_stmt|;
name|s1
operator|=
name|XDL_MAX
argument_list|(
name|xch
operator|->
name|i1
operator|-
name|xecfg
operator|->
name|ctxlen
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|s2
operator|=
name|XDL_MAX
argument_list|(
name|xch
operator|->
name|i2
operator|-
name|xecfg
operator|->
name|ctxlen
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|lctx
operator|=
name|xecfg
operator|->
name|ctxlen
expr_stmt|;
name|lctx
operator|=
name|XDL_MIN
argument_list|(
name|lctx
argument_list|,
name|xe
operator|->
name|xdf1
operator|.
name|nrec
operator|-
operator|(
name|xche
operator|->
name|i1
operator|+
name|xche
operator|->
name|chg1
operator|)
argument_list|)
expr_stmt|;
name|lctx
operator|=
name|XDL_MIN
argument_list|(
name|lctx
argument_list|,
name|xe
operator|->
name|xdf2
operator|.
name|nrec
operator|-
operator|(
name|xche
operator|->
name|i2
operator|+
name|xche
operator|->
name|chg2
operator|)
argument_list|)
expr_stmt|;
name|e1
operator|=
name|xche
operator|->
name|i1
operator|+
name|xche
operator|->
name|chg1
operator|+
name|lctx
expr_stmt|;
name|e2
operator|=
name|xche
operator|->
name|i2
operator|+
name|xche
operator|->
name|chg2
operator|+
name|lctx
expr_stmt|;
comment|/* 		 * Emit current hunk header. 		 */
if|if
condition|(
name|xdl_emit_hunk_hdr
argument_list|(
name|s1
operator|+
literal|1
argument_list|,
name|e1
operator|-
name|s1
argument_list|,
name|s2
operator|+
literal|1
argument_list|,
name|e2
operator|-
name|s2
argument_list|,
name|ecb
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
comment|/* 		 * Emit pre-context. 		 */
for|for
control|(
init|;
name|s1
operator|<
name|xch
operator|->
name|i1
condition|;
name|s1
operator|++
control|)
if|if
condition|(
name|xdl_emit_record
argument_list|(
operator|&
name|xe
operator|->
name|xdf1
argument_list|,
name|s1
argument_list|,
literal|" "
argument_list|,
name|ecb
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
for|for
control|(
name|s1
operator|=
name|xch
operator|->
name|i1
operator|,
name|s2
operator|=
name|xch
operator|->
name|i2
init|;
condition|;
name|xch
operator|=
name|xch
operator|->
name|next
control|)
block|{
comment|/* 			 * Merge previous with current change atom. 			 */
for|for
control|(
init|;
name|s1
operator|<
name|xch
operator|->
name|i1
operator|&&
name|s2
operator|<
name|xch
operator|->
name|i2
condition|;
name|s1
operator|++
operator|,
name|s2
operator|++
control|)
if|if
condition|(
name|xdl_emit_record
argument_list|(
operator|&
name|xe
operator|->
name|xdf1
argument_list|,
name|s1
argument_list|,
literal|" "
argument_list|,
name|ecb
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
comment|/* 			 * Removes lines from the first file. 			 */
for|for
control|(
name|s1
operator|=
name|xch
operator|->
name|i1
init|;
name|s1
operator|<
name|xch
operator|->
name|i1
operator|+
name|xch
operator|->
name|chg1
condition|;
name|s1
operator|++
control|)
if|if
condition|(
name|xdl_emit_record
argument_list|(
operator|&
name|xe
operator|->
name|xdf1
argument_list|,
name|s1
argument_list|,
literal|"-"
argument_list|,
name|ecb
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
comment|/* 			 * Adds lines from the second file. 			 */
for|for
control|(
name|s2
operator|=
name|xch
operator|->
name|i2
init|;
name|s2
operator|<
name|xch
operator|->
name|i2
operator|+
name|xch
operator|->
name|chg2
condition|;
name|s2
operator|++
control|)
if|if
condition|(
name|xdl_emit_record
argument_list|(
operator|&
name|xe
operator|->
name|xdf2
argument_list|,
name|s2
argument_list|,
literal|"+"
argument_list|,
name|ecb
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|xch
operator|==
name|xche
condition|)
break|break;
name|s1
operator|=
name|xch
operator|->
name|i1
operator|+
name|xch
operator|->
name|chg1
expr_stmt|;
name|s2
operator|=
name|xch
operator|->
name|i2
operator|+
name|xch
operator|->
name|chg2
expr_stmt|;
block|}
comment|/* 		 * Emit post-context. 		 */
for|for
control|(
name|s1
operator|=
name|xche
operator|->
name|i1
operator|+
name|xche
operator|->
name|chg1
init|;
name|s1
operator|<
name|e1
condition|;
name|s1
operator|++
control|)
if|if
condition|(
name|xdl_emit_record
argument_list|(
operator|&
name|xe
operator|->
name|xdf1
argument_list|,
name|s1
argument_list|,
literal|" "
argument_list|,
name|ecb
argument_list|)
operator|<
literal|0
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
