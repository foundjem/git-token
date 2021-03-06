begin_unit
begin_comment
comment|/*  *  LibXDiff by Davide Libenzi ( File Differential Library )  *  Copyright (C) 2003  Davide Libenzi  *  *  This library is free software; you can redistribute it and/or  *  modify it under the terms of the GNU Lesser General Public  *  License as published by the Free Software Foundation; either  *  version 2.1 of the License, or (at your option) any later version.  *  *  This library is distributed in the hope that it will be useful,  *  but WITHOUT ANY WARRANTY; without even the implied warranty of  *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU  *  Lesser General Public License for more details.  *  *  You should have received a copy of the GNU Lesser General Public  *  License along with this library; if not, write to the Free Software  *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  *  *  Davide Libenzi<davidel@xmailserver.org>  *  */
end_comment
begin_if
if|#
directive|if
operator|!
name|defined
argument_list|(
name|XUTILS_H
argument_list|)
end_if
begin_define
DECL|macro|XUTILS_H
define|#
directive|define
name|XUTILS_H
end_define
begin_function_decl
name|long
name|xdl_bogosqrt
parameter_list|(
name|long
name|n
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
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
function_decl|;
end_function_decl
begin_function_decl
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
function_decl|;
end_function_decl
begin_function_decl
name|void
name|xdl_cha_free
parameter_list|(
name|chastore_t
modifier|*
name|cha
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
modifier|*
name|xdl_cha_alloc
parameter_list|(
name|chastore_t
modifier|*
name|cha
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|long
name|xdl_guess_lines
parameter_list|(
name|mmfile_t
modifier|*
name|mf
parameter_list|,
name|long
name|sample
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|xdl_blankline
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|long
name|size
parameter_list|,
name|long
name|flags
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|xdl_recmatch
parameter_list|(
specifier|const
name|char
modifier|*
name|l1
parameter_list|,
name|long
name|s1
parameter_list|,
specifier|const
name|char
modifier|*
name|l2
parameter_list|,
name|long
name|s2
parameter_list|,
name|long
name|flags
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
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
parameter_list|,
name|long
name|flags
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|unsigned
name|int
name|xdl_hashbits
parameter_list|(
name|unsigned
name|int
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
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
function_decl|;
end_function_decl
begin_function_decl
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
specifier|const
name|char
modifier|*
name|func
parameter_list|,
name|long
name|funclen
parameter_list|,
name|xdemitcb_t
modifier|*
name|ecb
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|xdl_fall_back_diff
parameter_list|(
name|xdfenv_t
modifier|*
name|diff_env
parameter_list|,
name|xpparam_t
specifier|const
modifier|*
name|xpp
parameter_list|,
name|int
name|line1
parameter_list|,
name|int
name|count1
parameter_list|,
name|int
name|line2
parameter_list|,
name|int
name|count2
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* #if !defined(XUTILS_H) */
end_comment
end_unit
