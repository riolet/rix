typedef struct _$_OBJ_TYPE_R OBJ_TYPE_R;
typedef struct _$_Object_R Object_R;
_$_mptr* OBJ_TYPE_R_$_destructor_$_(  _$_mptr * _$_mptr_in);
_$_mptr* OBJ_TYPE_R_$_OBJ_TYPE_R_$_(  _$_mptr * _$_mptr_in);
_$_mptr* Object_R_$_Object_R_$_String_$_String_$_Object_R_$_Object_R_$_OBJ_TYPE_R_$_String_$_String_$_int_$_String_$_List_$_List_$_List_$_int(_$_mptr * name,_$_mptr * fullname,_$_mptr * parentClass,_$_mptr * parentScope,_$_mptr * category,_$_mptr * returnType,_$_mptr * genericType,int genericTypeArgPos,_$_mptr * resolvedSpecificType,List paramTypes,List definedSymbols,List code,int flags, _$_mptr * _$_mptr_in);
_$_mptr* Object_R_$_destructor_$_(  _$_mptr * _$_mptr_in);
_$_mptr* Object_R_$_Object_R_$_(  _$_mptr * _$_mptr_in);
struct _$_OBJ_TYPE_R {
	_$_mptr* $super;
	BaseType * $super_;
	int a;
};
_$_mptr* OBJ_TYPE_R_$_destructor_$_(  _$_mptr * _$_mptr_in) {
 	_$_cleanup(((OBJ_TYPE_R *)_$_mptr_in->obj)->$super);
 	free(((OBJ_TYPE_R *)_$_mptr_in->obj)->$super);
 	free(((OBJ_TYPE_R *)_$_mptr_in->obj));
}
_$_mptr* OBJ_TYPE_R_$_OBJ_TYPE_R_$_(  _$_mptr * _$_mptr_in) {
	_$_HEAP_VARIABLE(_$_mptr0);
 	OBJ_TYPE_R * $self_ = calloc(1, sizeof(OBJ_TYPE_R));
_$_mptr * $self = _$_returnAppointer(_$_mptr_in,$self_,OBJ_TYPE_R_$_destructor_$_);
 	$self_->$super= BaseType_$_BaseType_$_(_$_mptr0);
 	$self_->$super_= $self_->$super->obj;
 	return $self;
}
struct _$_Object_R {
	_$_mptr* $super;
	BaseType * $super_;
	Array arr;
	_$_mptr* name;
	_$_mptr* fullname;
	_$_mptr* parentClass;
	_$_mptr* parentScope;
	_$_mptr* category;
	_$_mptr* returnType;
	_$_mptr* genericType;
	int genericTypeArgPos;
	_$_mptr* resolvedSpecificType;
	List paramTypes;
	List definedSymbols;
	List code;
	int flags;
};
_$_mptr* Object_R_$_Object_R_$_String_$_String_$_Object_R_$_Object_R_$_OBJ_TYPE_R_$_String_$_String_$_int_$_String_$_List_$_List_$_List_$_int(_$_mptr * name,_$_mptr * fullname,_$_mptr * parentClass,_$_mptr * parentScope,_$_mptr * category,_$_mptr * returnType,_$_mptr * genericType,int genericTypeArgPos,_$_mptr * resolvedSpecificType,List paramTypes,List definedSymbols,List code,int flags, _$_mptr * _$_mptr_in) {
	_$_HEAP_VARIABLE(_$_mptr1);
	_$_HEAP_VARIABLE(_$_mptr2);
	_$_HEAP_VARIABLE(_$_mptr3);
	_$_HEAP_VARIABLE(_$_mptr4);
	_$_HEAP_VARIABLE(_$_mptr5);
	_$_HEAP_VARIABLE(_$_mptr6);
	_$_HEAP_VARIABLE(_$_mptr7);
	_$_HEAP_VARIABLE(_$_mptr8);
	_$_HEAP_VARIABLE(_$_mptr9);
 	Object_R * $self_ = calloc(1, sizeof(Object_R));
_$_mptr * $self = _$_returnAppointer(_$_mptr_in,$self_,Object_R_$_destructor_$_);
 	$self_->$super= BaseType_$_BaseType_$_(_$_mptr1);
 	$self_->$super_= $self_->$super->obj;
 	$self_->name= String_$_String_$_(_$_mptr2);
 	$self_->fullname= String_$_String_$_(_$_mptr3);
 	$self_->parentClass= Object_R_$_Object_R_$_(_$_mptr4);
 	$self_->parentScope= Object_R_$_Object_R_$_(_$_mptr5);
 	$self_->category= OBJ_TYPE_R_$_OBJ_TYPE_R_$_(_$_mptr6);
 	$self_->returnType= String_$_String_$_(_$_mptr7);
 	$self_->genericType= String_$_String_$_(_$_mptr8);
 	$self_->resolvedSpecificType= String_$_String_$_(_$_mptr9);
 	/*rixc.c 566*/ IDENT_MPTR_RAW_assign(/* 1748 */ (((Object_R *)( $self->obj)))->name,name);
 	/*rixc.c 566*/ IDENT_MPTR_RAW_assign(/* 1748 */ (((Object_R *)( $self->obj)))->fullname,fullname);
 	/*rixc.c 566*/ IDENT_MPTR_RAW_assign(/* 1748 */ (((Object_R *)( $self->obj)))->parentClass,parentClass);
 	/*rixc.c 566*/ IDENT_MPTR_RAW_assign(/* 1748 */ (((Object_R *)( $self->obj)))->parentScope,parentScope);
 	/*rixc.c 566*/ IDENT_MPTR_RAW_assign(/* 1748 */ (((Object_R *)( $self->obj)))->category,category);
 	/*rixc.c 566*/ IDENT_MPTR_RAW_assign(/* 1748 */ (((Object_R *)( $self->obj)))->returnType,returnType);
 	/*rixc.c 566*/ IDENT_MPTR_RAW_assign(/* 1748 */ (((Object_R *)( $self->obj)))->genericType,genericType);
 	/*rixc.c 566*/ /* 1748 */ (((Object_R *)( $self->obj)))->genericTypeArgPos = genericTypeArgPos;
 	/*rixc.c 566*/ IDENT_MPTR_RAW_assign(/* 1748 */ (((Object_R *)( $self->obj)))->resolvedSpecificType,resolvedSpecificType);
 	/*rixc.c 566*/ /* 1748 */ (((Object_R *)( $self->obj)))->paramTypes = paramTypes;
 	/*rixc.c 566*/ /* 1748 */ (((Object_R *)( $self->obj)))->definedSymbols = definedSymbols;
 	/*rixc.c 566*/ /* 1748 */ (((Object_R *)( $self->obj)))->code = code;
 	/*rixc.c 566*/ /* 1748 */ (((Object_R *)( $self->obj)))->flags = flags;
 	return $self;
}
_$_mptr* Object_R_$_destructor_$_(  _$_mptr * _$_mptr_in) {
 	_$_cleanup(((Object_R *)_$_mptr_in->obj)->$super);
 	free(((Object_R *)_$_mptr_in->obj)->$super);
 	_$_cleanup(((Object_R *)_$_mptr_in->obj)->name);
 	free(((Object_R *)_$_mptr_in->obj)->name);
 	_$_cleanup(((Object_R *)_$_mptr_in->obj)->fullname);
 	free(((Object_R *)_$_mptr_in->obj)->fullname);
 	_$_cleanup(((Object_R *)_$_mptr_in->obj)->parentClass);
 	free(((Object_R *)_$_mptr_in->obj)->parentClass);
 	_$_cleanup(((Object_R *)_$_mptr_in->obj)->parentScope);
 	free(((Object_R *)_$_mptr_in->obj)->parentScope);
 	_$_cleanup(((Object_R *)_$_mptr_in->obj)->category);
 	free(((Object_R *)_$_mptr_in->obj)->category);
 	_$_cleanup(((Object_R *)_$_mptr_in->obj)->returnType);
 	free(((Object_R *)_$_mptr_in->obj)->returnType);
 	_$_cleanup(((Object_R *)_$_mptr_in->obj)->genericType);
 	free(((Object_R *)_$_mptr_in->obj)->genericType);
 	_$_cleanup(((Object_R *)_$_mptr_in->obj)->resolvedSpecificType);
 	free(((Object_R *)_$_mptr_in->obj)->resolvedSpecificType);
 	free(((Object_R *)_$_mptr_in->obj));
}
_$_mptr* Object_R_$_Object_R_$_(  _$_mptr * _$_mptr_in) {
	_$_HEAP_VARIABLE(_$_mptr10);
	_$_HEAP_VARIABLE(_$_mptr11);
	_$_HEAP_VARIABLE(_$_mptr12);
	_$_HEAP_VARIABLE(_$_mptr13);
	_$_HEAP_VARIABLE(_$_mptr14);
	_$_HEAP_VARIABLE(_$_mptr15);
	_$_HEAP_VARIABLE(_$_mptr16);
	_$_HEAP_VARIABLE(_$_mptr17);
	_$_HEAP_VARIABLE(_$_mptr18);
 	Object_R * $self_ = calloc(1, sizeof(Object_R));
_$_mptr * $self = _$_returnAppointer(_$_mptr_in,$self_,Object_R_$_destructor_$_);
 	$self_->$super= BaseType_$_BaseType_$_(_$_mptr10);
 	$self_->$super_= $self_->$super->obj;
 	$self_->name= String_$_String_$_(_$_mptr11);
 	$self_->fullname= String_$_String_$_(_$_mptr12);
 	$self_->parentClass= Object_R_$_Object_R_$_(_$_mptr13);
 	$self_->parentScope= Object_R_$_Object_R_$_(_$_mptr14);
 	$self_->category= OBJ_TYPE_R_$_OBJ_TYPE_R_$_(_$_mptr15);
 	$self_->returnType= String_$_String_$_(_$_mptr16);
 	$self_->genericType= String_$_String_$_(_$_mptr17);
 	$self_->resolvedSpecificType= String_$_String_$_(_$_mptr18);
 	return $self;
}
