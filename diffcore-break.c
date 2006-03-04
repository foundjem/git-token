begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  */
end_comment
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
begin_function
DECL|function|should_break
specifier|static
name|int
name|should_break
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
name|int
name|break_score
parameter_list|,
name|int
modifier|*
name|merge_score_p
parameter_list|)
block|{
comment|/* dst is recorded as a modification of src.  Are they so 	 * different that we are better off recording this as a pair 	 * of delete and create? 	 * 	 * There are two criteria used in this algorithm.  For the 	 * purposes of helping later rename/copy, we take both delete 	 * and insert into account and estimate the amount of "edit". 	 * If the edit is very large, we break this pair so that 	 * rename/copy can pick the pieces up to match with other 	 * files. 	 * 	 * On the other hand, we would want to ignore inserts for the 	 * pure "complete rewrite" detection.  As long as most of the 	 * existing contents were removed from the file, it is a 	 * complete rewrite, and if sizable chunk from the original 	 * still remains in the result, it is not a rewrite.  It does 	 * not matter how much or how little new material is added to 	 * the file. 	 * 	 * The score we leave for such a broken filepair uses the 	 * latter definition so that later clean-up stage can find the 	 * pieces that should not have been broken according to the 	 * latter definition after rename/copy runs, and merge the 	 * broken pair that have a score lower than given criteria 	 * back together.  The break operation itself happens 	 * according to the former definition. 	 * 	 * The minimum_edit parameter tells us when to break (the 	 * amount of "edit" required for us to consider breaking the 	 * pair).  We leave the amount of deletion in *merge_score_p 	 * when we return. 	 * 	 * The value we return is 1 if we want the pair to be broken, 	 * or 0 if we do not. 	 */
name|unsigned
name|long
name|delta_size
decl_stmt|,
name|base_size
decl_stmt|,
name|src_copied
decl_stmt|,
name|literal_added
decl_stmt|,
name|src_removed
decl_stmt|;
operator|*
name|merge_score_p
operator|=
literal|0
expr_stmt|;
comment|/* assume no deletion --- "do not break" 			     * is the default. 			     */
if|if
condition|(
operator|!
name|S_ISREG
argument_list|(
name|src
operator|->
name|mode
argument_list|)
operator|||
operator|!
name|S_ISREG
argument_list|(
name|dst
operator|->
name|mode
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* leave symlink rename alone */
if|if
condition|(
name|src
operator|->
name|sha1_valid
operator|&&
name|dst
operator|->
name|sha1_valid
operator|&&
operator|!
name|memcmp
argument_list|(
name|src
operator|->
name|sha1
argument_list|,
name|dst
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* they are the same */
if|if
condition|(
name|diff_populate_filespec
argument_list|(
name|src
argument_list|,
literal|0
argument_list|)
operator|||
name|diff_populate_filespec
argument_list|(
name|dst
argument_list|,
literal|0
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* error but caught downstream */
name|base_size
operator|=
operator|(
operator|(
name|src
operator|->
name|size
operator|<
name|dst
operator|->
name|size
operator|)
condition|?
name|src
operator|->
name|size
else|:
name|dst
operator|->
name|size
operator|)
expr_stmt|;
if|if
condition|(
name|base_size
operator|<
name|MINIMUM_BREAK_SIZE
condition|)
return|return
literal|0
return|;
comment|/* we do not break too small filepair */
if|if
condition|(
name|diffcore_count_changes
argument_list|(
name|src
operator|->
name|data
argument_list|,
name|src
operator|->
name|size
argument_list|,
name|dst
operator|->
name|data
argument_list|,
name|dst
operator|->
name|size
argument_list|,
literal|0
argument_list|,
operator|&
name|src_copied
argument_list|,
operator|&
name|literal_added
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* sanity */
if|if
condition|(
name|src
operator|->
name|size
operator|<
name|src_copied
condition|)
name|src_copied
operator|=
name|src
operator|->
name|size
expr_stmt|;
if|if
condition|(
name|dst
operator|->
name|size
operator|<
name|literal_added
operator|+
name|src_copied
condition|)
block|{
if|if
condition|(
name|src_copied
operator|<
name|dst
operator|->
name|size
condition|)
name|literal_added
operator|=
name|dst
operator|->
name|size
operator|-
name|src_copied
expr_stmt|;
else|else
name|literal_added
operator|=
literal|0
expr_stmt|;
block|}
name|src_removed
operator|=
name|src
operator|->
name|size
operator|-
name|src_copied
expr_stmt|;
comment|/* Compute merge-score, which is "how much is removed 	 * from the source material".  The clean-up stage will 	 * merge the surviving pair together if the score is 	 * less than the minimum, after rename/copy runs. 	 */
operator|*
name|merge_score_p
operator|=
name|src_removed
operator|*
name|MAX_SCORE
operator|/
name|src
operator|->
name|size
expr_stmt|;
comment|/* Extent of damage, which counts both inserts and 	 * deletes. 	 */
name|delta_size
operator|=
name|src_removed
operator|+
name|literal_added
expr_stmt|;
if|if
condition|(
name|delta_size
operator|*
name|MAX_SCORE
operator|/
name|base_size
operator|<
name|break_score
condition|)
return|return
literal|0
return|;
comment|/* If you removed a lot without adding new material, that is 	 * not really a rewrite. 	 */
if|if
condition|(
operator|(
name|src
operator|->
name|size
operator|*
name|break_score
operator|<
name|src_removed
operator|*
name|MAX_SCORE
operator|)
operator|&&
operator|(
name|literal_added
operator|*
literal|20
operator|<
name|src_removed
operator|)
operator|&&
operator|(
name|literal_added
operator|*
literal|20
operator|<
name|src_copied
operator|)
condition|)
return|return
literal|0
return|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|diffcore_break
name|void
name|diffcore_break
parameter_list|(
name|int
name|break_score
parameter_list|)
block|{
name|struct
name|diff_queue_struct
modifier|*
name|q
init|=
operator|&
name|diff_queued_diff
decl_stmt|;
name|struct
name|diff_queue_struct
name|outq
decl_stmt|;
comment|/* When the filepair has this much edit (insert and delete), 	 * it is first considered to be a rewrite and broken into a 	 * create and delete filepair.  This is to help breaking a 	 * file that had too much new stuff added, possibly from 	 * moving contents from another file, so that rename/copy can 	 * match it with the other file. 	 * 	 * int break_score; we reuse incoming parameter for this. 	 */
comment|/* After a pair is broken according to break_score and 	 * subjected to rename/copy, both of them may survive intact, 	 * due to lack of suitable rename/copy peer.  Or, the caller 	 * may be calling us without using rename/copy.  When that 	 * happens, we merge the broken pieces back into one 	 * modification together if the pair did not have more than 	 * this much delete.  For this computation, we do not take 	 * insert into account at all.  If you start from a 100-line 	 * file and delete 97 lines of it, it does not matter if you 	 * add 27 lines to it to make a new 30-line file or if you add 	 * 997 lines to it to make a 1000-line file.  Either way what 	 * you did was a rewrite of 97%.  On the other hand, if you 	 * delete 3 lines, keeping 97 lines intact, it does not matter 	 * if you add 3 lines to it to make a new 100-line file or if 	 * you add 903 lines to it to make a new 1000-line file. 	 * Either way you did a lot of additions and not a rewrite. 	 * This merge happens to catch the latter case.  A merge_score 	 * of 80% would be a good default value (a broken pair that 	 * has score lower than merge_score will be merged back 	 * together). 	 */
name|int
name|merge_score
decl_stmt|;
name|int
name|i
decl_stmt|;
comment|/* See comment on DEFAULT_BREAK_SCORE and 	 * DEFAULT_MERGE_SCORE in diffcore.h 	 */
name|merge_score
operator|=
operator|(
name|break_score
operator|>>
literal|16
operator|)
operator|&
literal|0xFFFF
expr_stmt|;
name|break_score
operator|=
operator|(
name|break_score
operator|&
literal|0xFFFF
operator|)
expr_stmt|;
if|if
condition|(
operator|!
name|break_score
condition|)
name|break_score
operator|=
name|DEFAULT_BREAK_SCORE
expr_stmt|;
if|if
condition|(
operator|!
name|merge_score
condition|)
name|merge_score
operator|=
name|DEFAULT_MERGE_SCORE
expr_stmt|;
name|outq
operator|.
name|nr
operator|=
name|outq
operator|.
name|alloc
operator|=
literal|0
expr_stmt|;
name|outq
operator|.
name|queue
operator|=
name|NULL
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|diff_filepair
modifier|*
name|p
init|=
name|q
operator|->
name|queue
index|[
name|i
index|]
decl_stmt|;
name|int
name|score
decl_stmt|;
comment|/* We deal only with in-place edit of non directory. 		 * We do not break anything else. 		 */
if|if
condition|(
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|one
argument_list|)
operator|&&
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|two
argument_list|)
operator|&&
operator|!
name|S_ISDIR
argument_list|(
name|p
operator|->
name|one
operator|->
name|mode
argument_list|)
operator|&&
operator|!
name|S_ISDIR
argument_list|(
name|p
operator|->
name|two
operator|->
name|mode
argument_list|)
operator|&&
operator|!
name|strcmp
argument_list|(
name|p
operator|->
name|one
operator|->
name|path
argument_list|,
name|p
operator|->
name|two
operator|->
name|path
argument_list|)
condition|)
block|{
if|if
condition|(
name|should_break
argument_list|(
name|p
operator|->
name|one
argument_list|,
name|p
operator|->
name|two
argument_list|,
name|break_score
argument_list|,
operator|&
name|score
argument_list|)
condition|)
block|{
comment|/* Split this into delete and create */
name|struct
name|diff_filespec
modifier|*
name|null_one
decl_stmt|,
modifier|*
name|null_two
decl_stmt|;
name|struct
name|diff_filepair
modifier|*
name|dp
decl_stmt|;
comment|/* Set score to 0 for the pair that 				 * needs to be merged back together 				 * should they survive rename/copy. 				 * Also we do not want to break very 				 * small files. 				 */
if|if
condition|(
name|score
operator|<
name|merge_score
condition|)
name|score
operator|=
literal|0
expr_stmt|;
comment|/* deletion of one */
name|null_one
operator|=
name|alloc_filespec
argument_list|(
name|p
operator|->
name|one
operator|->
name|path
argument_list|)
expr_stmt|;
name|dp
operator|=
name|diff_queue
argument_list|(
operator|&
name|outq
argument_list|,
name|p
operator|->
name|one
argument_list|,
name|null_one
argument_list|)
expr_stmt|;
name|dp
operator|->
name|score
operator|=
name|score
expr_stmt|;
name|dp
operator|->
name|broken_pair
operator|=
literal|1
expr_stmt|;
comment|/* creation of two */
name|null_two
operator|=
name|alloc_filespec
argument_list|(
name|p
operator|->
name|two
operator|->
name|path
argument_list|)
expr_stmt|;
name|dp
operator|=
name|diff_queue
argument_list|(
operator|&
name|outq
argument_list|,
name|null_two
argument_list|,
name|p
operator|->
name|two
argument_list|)
expr_stmt|;
name|dp
operator|->
name|score
operator|=
name|score
expr_stmt|;
name|dp
operator|->
name|broken_pair
operator|=
literal|1
expr_stmt|;
name|free
argument_list|(
name|p
argument_list|)
expr_stmt|;
comment|/* not diff_free_filepair(), we are 					  * reusing one and two here. 					  */
continue|continue;
block|}
block|}
name|diff_q
argument_list|(
operator|&
name|outq
argument_list|,
name|p
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|q
operator|->
name|queue
argument_list|)
expr_stmt|;
operator|*
name|q
operator|=
name|outq
expr_stmt|;
return|return;
block|}
end_function
begin_function
DECL|function|merge_broken
specifier|static
name|void
name|merge_broken
parameter_list|(
name|struct
name|diff_filepair
modifier|*
name|p
parameter_list|,
name|struct
name|diff_filepair
modifier|*
name|pp
parameter_list|,
name|struct
name|diff_queue_struct
modifier|*
name|outq
parameter_list|)
block|{
comment|/* p and pp are broken pairs we want to merge */
name|struct
name|diff_filepair
modifier|*
name|c
init|=
name|p
decl_stmt|,
modifier|*
name|d
init|=
name|pp
decl_stmt|,
modifier|*
name|dp
decl_stmt|;
if|if
condition|(
name|DIFF_FILE_VALID
argument_list|(
name|p
operator|->
name|one
argument_list|)
condition|)
block|{
comment|/* this must be a delete half */
name|d
operator|=
name|p
expr_stmt|;
name|c
operator|=
name|pp
expr_stmt|;
block|}
comment|/* Sanity check */
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|d
operator|->
name|one
argument_list|)
condition|)
name|die
argument_list|(
literal|"internal error in merge #1"
argument_list|)
expr_stmt|;
if|if
condition|(
name|DIFF_FILE_VALID
argument_list|(
name|d
operator|->
name|two
argument_list|)
condition|)
name|die
argument_list|(
literal|"internal error in merge #2"
argument_list|)
expr_stmt|;
if|if
condition|(
name|DIFF_FILE_VALID
argument_list|(
name|c
operator|->
name|one
argument_list|)
condition|)
name|die
argument_list|(
literal|"internal error in merge #3"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|DIFF_FILE_VALID
argument_list|(
name|c
operator|->
name|two
argument_list|)
condition|)
name|die
argument_list|(
literal|"internal error in merge #4"
argument_list|)
expr_stmt|;
name|dp
operator|=
name|diff_queue
argument_list|(
name|outq
argument_list|,
name|d
operator|->
name|one
argument_list|,
name|c
operator|->
name|two
argument_list|)
expr_stmt|;
name|dp
operator|->
name|score
operator|=
name|p
operator|->
name|score
expr_stmt|;
name|diff_free_filespec_data
argument_list|(
name|d
operator|->
name|two
argument_list|)
expr_stmt|;
name|diff_free_filespec_data
argument_list|(
name|c
operator|->
name|one
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|d
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|c
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|diffcore_merge_broken
name|void
name|diffcore_merge_broken
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|diff_queue_struct
modifier|*
name|q
init|=
operator|&
name|diff_queued_diff
decl_stmt|;
name|struct
name|diff_queue_struct
name|outq
decl_stmt|;
name|int
name|i
decl_stmt|,
name|j
decl_stmt|;
name|outq
operator|.
name|nr
operator|=
name|outq
operator|.
name|alloc
operator|=
literal|0
expr_stmt|;
name|outq
operator|.
name|queue
operator|=
name|NULL
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|diff_filepair
modifier|*
name|p
init|=
name|q
operator|->
name|queue
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|p
condition|)
comment|/* we already merged this with its peer */
continue|continue;
elseif|else
if|if
condition|(
name|p
operator|->
name|broken_pair
operator|&&
operator|!
name|strcmp
argument_list|(
name|p
operator|->
name|one
operator|->
name|path
argument_list|,
name|p
operator|->
name|two
operator|->
name|path
argument_list|)
condition|)
block|{
comment|/* If the peer also survived rename/copy, then 			 * we merge them back together. 			 */
for|for
control|(
name|j
operator|=
name|i
operator|+
literal|1
init|;
name|j
operator|<
name|q
operator|->
name|nr
condition|;
name|j
operator|++
control|)
block|{
name|struct
name|diff_filepair
modifier|*
name|pp
init|=
name|q
operator|->
name|queue
index|[
name|j
index|]
decl_stmt|;
if|if
condition|(
name|pp
operator|->
name|broken_pair
operator|&&
operator|!
name|strcmp
argument_list|(
name|pp
operator|->
name|one
operator|->
name|path
argument_list|,
name|pp
operator|->
name|two
operator|->
name|path
argument_list|)
operator|&&
operator|!
name|strcmp
argument_list|(
name|p
operator|->
name|one
operator|->
name|path
argument_list|,
name|pp
operator|->
name|two
operator|->
name|path
argument_list|)
condition|)
block|{
comment|/* Peer survived.  Merge them */
name|merge_broken
argument_list|(
name|p
argument_list|,
name|pp
argument_list|,
operator|&
name|outq
argument_list|)
expr_stmt|;
name|q
operator|->
name|queue
index|[
name|j
index|]
operator|=
name|NULL
expr_stmt|;
break|break;
block|}
block|}
if|if
condition|(
name|q
operator|->
name|nr
operator|<=
name|j
condition|)
comment|/* The peer did not survive, so we keep 				 * it in the output. 				 */
name|diff_q
argument_list|(
operator|&
name|outq
argument_list|,
name|p
argument_list|)
expr_stmt|;
block|}
else|else
name|diff_q
argument_list|(
operator|&
name|outq
argument_list|,
name|p
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|q
operator|->
name|queue
argument_list|)
expr_stmt|;
operator|*
name|q
operator|=
name|outq
expr_stmt|;
return|return;
block|}
end_function
end_unit
