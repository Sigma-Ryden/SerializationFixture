// If you want to undef all external lib's macros
// you should include given file only after library files

// clean up
#undef SERIALIZATION_ACCESS

#undef EXPORT_SERIALIZATION_ARCHIVE

#undef INSTANTIABLE_TYPE

#undef EXPORT_INSTANTIABLE_KEY
#undef EXPORT_INSTANTIABLE

#undef SERIALIZATION
#undef TEMPLATE_SERIALIZATION
#undef CONDITIONAL_SERIALIZATION
#undef SERIALIZATION_DECLARATION
#undef SERIALIZATION_DEFINITION

#undef BITPACK

#undef SF_STRING_HASH
#undef SF_TYPE_HASH
#undef SF_EXPRESSION_HASH

#undef SF_ARCHIVE_TRAIT_MAX_KEY_SIZE
#undef SF_BYTE_STREAM_RESERVE_SIZE

// possible to undef
// #undef SF_PTRTRACK_DISABLE
// #undef SF_DEFAULT_DISABLE
// #undef SF_TYPE_PORTABLE_DISABLE
// #undef SF_ANY_SUPPORT_DISABLE
// #undef SF_GARBAGE_CHECK_DISABLE

#undef SF_REQUIRES