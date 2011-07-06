begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|OBJECT_H
end_ifndef
begin_define
DECL|macro|OBJECT_H
define|#
directive|define
name|OBJECT_H
end_define
begin_struct
DECL|struct|object_list
struct|struct
name|object_list
block|{
DECL|member|item
name|struct
name|object
modifier|*
name|item
decl_stmt|;
DECL|member|next
name|struct
name|object_list
modifier|*
name|next
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|object_array
struct|struct
name|object_array
block|{
DECL|member|nr
name|unsigned
name|int
name|nr
decl_stmt|;
DECL|member|alloc
name|unsigned
name|int
name|alloc
decl_stmt|;
DECL|struct|object_array_entry
struct|struct
name|object_array_entry
block|{
DECL|member|item
name|struct
name|object
modifier|*
name|item
decl_stmt|;
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|mode
name|unsigned
name|mode
decl_stmt|;
block|}
DECL|member|objects
modifier|*
name|objects
struct|;
block|}
struct|;
end_struct
begin_define
DECL|macro|OBJECT_ARRAY_INIT
define|#
directive|define
name|OBJECT_ARRAY_INIT
value|{ 0, 0, NULL }
end_define
begin_define
DECL|macro|TYPE_BITS
define|#
directive|define
name|TYPE_BITS
value|3
end_define
begin_define
DECL|macro|FLAG_BITS
define|#
directive|define
name|FLAG_BITS
value|27
end_define
begin_comment
comment|/*  * The object type is stored in 3 bits.  */
end_comment
begin_struct
DECL|struct|object
struct|struct
name|object
block|{
DECL|member|parsed
name|unsigned
name|parsed
range|:
literal|1
decl_stmt|;
DECL|member|used
name|unsigned
name|used
range|:
literal|1
decl_stmt|;
DECL|member|type
name|unsigned
name|type
range|:
name|TYPE_BITS
decl_stmt|;
DECL|member|flags
name|unsigned
name|flags
range|:
name|FLAG_BITS
decl_stmt|;
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
specifier|const
name|char
modifier|*
name|typename
parameter_list|(
name|unsigned
name|int
name|type
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|type_from_string
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|unsigned
name|int
name|get_max_object_index
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|object
modifier|*
name|get_indexed_object
parameter_list|(
name|unsigned
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * This can be used to see if we have heard of the object before, but  * it can return "yes we have, and here is a half-initialised object"  * for an object that we haven't loaded/parsed yet.  *  * When parsing a commit to create an in-core commit object, its  * parents list holds commit objects that represent its parents, but  * they are expected to be lazily initialized and do not know what  * their trees or parents are yet.  When this function returns such a  * half-initialised objects, the caller is expected to initialize them  * by calling parse_object() on them.  */
end_comment
begin_function_decl
name|struct
name|object
modifier|*
name|lookup_object
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
modifier|*
name|create_object
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|type
parameter_list|,
name|void
modifier|*
name|obj
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Returns the object, having parsed it to find out what it is. **/
end_comment
begin_function_decl
name|struct
name|object
modifier|*
name|parse_object
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Given the result of read_sha1_file(), returns the object after  * parsing it.  eaten_p indicates if the object has a borrowed copy  * of buffer and the caller should not free() it.  */
end_comment
begin_function_decl
name|struct
name|object
modifier|*
name|parse_object_buffer
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|enum
name|object_type
name|type
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|int
modifier|*
name|eaten_p
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Returns the object, with potentially excess memory allocated. **/
end_comment
begin_function_decl
name|struct
name|object
modifier|*
name|lookup_unknown_object
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|object_list
modifier|*
name|object_list_insert
parameter_list|(
name|struct
name|object
modifier|*
name|item
parameter_list|,
name|struct
name|object_list
modifier|*
modifier|*
name|list_p
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|object_list_contains
parameter_list|(
name|struct
name|object_list
modifier|*
name|list
parameter_list|,
name|struct
name|object
modifier|*
name|obj
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* Object array handling .. */
end_comment
begin_function_decl
name|void
name|add_object_array
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|object_array
modifier|*
name|array
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|add_object_array_with_mode
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|object_array
modifier|*
name|array
parameter_list|,
name|unsigned
name|mode
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|object_array_remove_duplicates
parameter_list|(
name|struct
name|object_array
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* OBJECT_H */
end_comment
end_unit
