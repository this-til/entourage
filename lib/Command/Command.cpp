// 
// 
// 

#include "Command.h"

_Command Command;

_Command::_Command() {
}

_Command::~_Command() {
}

/************************************************************************************************************
???? ?? ??????  Judgment		????У?麯??
?????????????	command	??		??????????
???? ?? ?????	??
????    ??????	Judgment(command01);	У??command01????
************************************************************************************************************/
void _Command::Judgment(uint8_t* command) {
    uint16_t tp = command[2] + command[3] + command[4] + command[5];
    command[6] = uint8_t(tp % 256);
}

//uint8_t command01[ 8 ] = { 0x55,0x01,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*?????????????*/
//
//uint8_t command02[ 8 ] = { 0x55,0x03,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*?????????*/
//uint8_t command03[ 8 ] = { 0x55,0x03,0x02 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*??????????*/
//
//uint8_t command04[ 8 ] = { 0x55,0x0A,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*???????????????*/
//uint8_t command05[ 8 ] = { 0x55,0x0A,0x02 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*????????????????*/
//
//uint8_t command06[ 8 ] = { 0x55,0x04,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*LED???????????*/
//uint8_t command07[ 8 ] = { 0x55,0x04,0x02 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*LED???????????*/
//uint8_t command08[ 8 ] = { 0x55,0x04,0x03 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*LED???????????*/
//uint8_t command09[ 8 ] = { 0x55,0x04,0x03 ,0x01 ,0x00 ,0x00 ,0x00 ,0xbb };	/*LED???????????*/
//uint8_t command10[ 8 ] = { 0x55,0x04,0x03 ,0x02 ,0x00 ,0x00 ,0x00 ,0xbb };	/*LED????????????*/
//uint8_t command11[ 8 ] = { 0x55,0x04,0x04 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*LED???????????*/
//
//uint8_t command12[ 8 ] = { 0x55,0x05,0x10 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*TFT?????????*/
//uint8_t command13[ 8 ] = { 0x55,0x05,0x20 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*TFT???????????*/
//uint8_t command14[ 8 ] = { 0x55,0x05,0x30 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*TFT??????????*/
//uint8_t command15[ 8 ] = { 0x55,0x05,0x30 ,0x01 ,0x00 ,0x00 ,0x00 ,0xbb };	/*TFT?????????*/
//uint8_t command16[ 8 ] = { 0x55,0x05,0x30 ,0x02 ,0x00 ,0x00 ,0x00 ,0xbb };	/*TFT???????????*/
//uint8_t command17[ 8 ] = { 0x55,0x05,0x40 ,0x02 ,0x00 ,0x00 ,0x00 ,0xbb };	/*TFT???HEX????????*/
//uint8_t command18[ 8 ] = { 0x55,0x05,0x50 ,0x00 ,0x00 ,0x00 ,0x00 ,0xbb };	/*TFT???????????*/


