begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|TREE_WALK_H
end_ifndef
begin_define
DECL|macro|TREE_WALK_H
define|#
directive|define
name|TREE_WALK_H
end_define
begin_struct
DECL|struct|tree_desc
struct|struct
name|tree_desc
block|{
DECL|member|buf
name|void
modifier|*
name|buf
decl_stmt|;
DECL|member|size
name|unsigned
name|long
name|size
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|name_entry
struct|struct
name|name_entry
block|{
DECL|member|sha1
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|;
DECL|member|path
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
DECL|member|mode
name|unsigned
name|int
name|mode
decl_stmt|;
DECL|member|pathlen
name|int
name|pathlen
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
name|void
name|update_tree_entry
parameter_list|(
name|struct
name|tree_desc
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|const
name|unsigned
name|char
modifier|*
name|tree_entry_extract
parameter_list|(
name|struct
name|tree_desc
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
parameter_list|,
name|unsigned
name|int
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
modifier|*
name|fill_tree_descriptor
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_typedef
DECL|typedef|traverse_callback_t
typedef|typedef
name|void
function_decl|(
modifier|*
name|traverse_callback_t
function_decl|)
parameter_list|(
name|int
name|n
parameter_list|,
name|unsigned
name|long
name|mask
parameter_list|,
name|struct
name|name_entry
modifier|*
name|entry
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
function_decl|;
end_typedef
begin_function_decl
name|void
name|traverse_trees
parameter_list|(
name|int
name|n
parameter_list|,
name|struct
name|tree_desc
modifier|*
name|t
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|traverse_callback_t
name|callback
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|get_tree_entry
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|,
name|unsigned
name|char
modifier|*
parameter_list|,
name|unsigned
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
