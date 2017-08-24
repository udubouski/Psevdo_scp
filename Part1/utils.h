#ifndef UTILS_H
#define UTILS_H

/*
 * Prints identifier of given sc-element.
 * If element has not identifier, prints its address in sc-memory
 * If element is arc prints it in (a -> b) form recursively.
*/
void printEl(sc_memory_context *context, sc_addr element);
/*
 * Prints content of given sc-link
*/
void printContent(sc_memory_context *context, sc_addr element);
/*
 * Resolves integer content of given sc-link
*/
int getInt(sc_memory_context *context, sc_addr element);
/*
 * Resolves float content of given sc-link
*/
int getFloat(sc_memory_context *context, sc_addr element);
/*
 * Creates sc-link with given integer content in sc-memory
*/
sc_addr genIntNode(sc_memory_context *context, sc_uint32 data);
/*
 * Creates sc-link with given float content in sc-memory
*/
sc_addr genFloatNode(sc_memory_context *context, float data);

#endif // UTILS_H
