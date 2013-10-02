#include <marsyas/expr/ExScanner.h>

#include <memory.h>
#include <string.h>

// string handling, wide character

char* coco_string_create(const char* value) {
  char* data;
  int len = 0;
  if (value) { len =(int)strlen(value); }
  data = new char[len + 1];
  strncpy(data, value, len);
  data[len] = 0;
  return data;
}

char* coco_string_create(const char* value , int startIndex, int length) {
  int len = 0;
  char* data;

  if (value) { len = length; }
  data = new char[len + 1];
  strncpy(data, &(value[startIndex]), len);
  data[len] = 0;

  return data;
}

char* coco_string_create_upper(char* data) {
  if (!data) { return NULL; }

  int dataLen = 0;
  if (data) { dataLen =(int)strlen(data); }

  char* newData = new char[dataLen + 1];

  for (int i = 0; i <= dataLen; ++i) {
    if (('a' <= data[i]) && (data[i] <= 'z')) {
      newData[i] = data[i] + ('A' - 'a');
    }
    else { newData[i] = data[i]; }
  }

  newData[dataLen] = '\0';
  return newData;
}

char* coco_string_create_lower(char* data) {
  if (!data) { return NULL; }

  int dataLen = 0;
  if (data) { dataLen =(int)strlen(data); }

  char* newData = new char[dataLen + 1];

  for (int i = 0; i <= dataLen; ++i) {
    if (('A' <= data[i]) && (data[i] <= 'Z')) {
      newData[i] = data[i] - ('A'- 'a');
    }
    else { newData[i] = data[i]; }
  }
  newData[dataLen] = '\0';
  return newData;
}

char* coco_string_create_append(const char* data1, const char* data2) {
  char* data;
  int data1Len = 0;
  int data2Len = 0;

  if (data1) { data1Len =(int)strlen(data1); }
  if (data2) {data2Len =(int)strlen(data2); }

  data = new char[data1Len + data2Len + 1];

  if (data1) { strcpy(data, data1); }
  if (data2) { strcpy(data + data1Len, data2); }

  data[data1Len + data2Len] = 0;

  return data;
}

char* coco_string_create_append(const char* target, const char appendix) {
  int targetLen = coco_string_length(target);
  char* data = new char[targetLen + 2];
  strncpy(data, target, targetLen);
  data[targetLen] = appendix;
  data[targetLen + 1] = 0;
  return data;
}

void coco_string_delete(char* &data) {
  delete [] data;
  data = NULL;
}

int coco_string_length(const char* data) {
  if (data) { return (int)strlen(data); }
  return 0;
}

bool coco_string_endswith(char* data, char* end) {
  int dataLen =(int)strlen(data);
  int endLen =(int)strlen(end);
  return (endLen <= dataLen) && (strcmp(data + dataLen - endLen, end) == 0);
}

int coco_string_indexof(char* data, char value) {
  char* chr = strchr(data, value);

  if (chr) { return (int)(chr-data); }
  return -1;
}

int coco_string_lastindexof(char* data, char value) {
  char* chr = strrchr(data, value);

  if (chr) { return (int)(chr-data); }
  return -1;
}

void coco_string_merge(char* &target, char* appendix) {
  if (!appendix) { return; }
  char* data = coco_string_create_append(target, appendix);
  delete [] target;
  target = data;
}

bool coco_string_equal(char* data1, char* data2) {
  return strcmp( data1, data2 ) == 0;
}

int coco_string_compareto(char* data1, char* data2) {
  return strcmp(data1, data2);
}

int coco_string_hash(char* data) {
  int h = 0;
  if (!data) { return 0; }
  while (*data != 0) {
    h = (h * 7) ^ *data;
    ++data;
  }
  if (h < 0) { h = -h; }
  return h;
}

using namespace Marsyas;

Token::Token() {
  kind = 0;
  pos  = 0;
  col  = 0;
  line = 0;
  val  = NULL;
  next = NULL;
}

Token::~Token() {
  coco_string_delete(val);
}

#include <iostream>
Buffer::Buffer(const char* s) {
  stream = NULL; // not a file
  this->isUserStream = true; // so Buffer doesn't try to close file
  int l; for (l=0; s[l]!='\0'; l++); // find string length
  fileLen = bufLen = l;
//    std::cout << "len=" << bufLen << std::endl;
  if (bufLen>MAX_BUFFER_LENGTH) { bufLen=MAX_BUFFER_LENGTH; }
  buf = new char[bufLen];
  for (int i=0; i<fileLen; ++i) { buf[i]=s[i]; }
  bufStart = 0; // set to start of buffer
  SetPos(0);          // setup  buffer to position 0 (start)
  if (bufLen == fileLen) Close();
}

Buffer::Buffer(FILE* s, bool isUserStream) {
  stream = s; this->isUserStream = isUserStream;
  fseek(s, 0, SEEK_END);
  fileLen = bufLen = ftell(s);
  fseek(s, 0, SEEK_SET);
  buf = new char[MAX_BUFFER_LENGTH];
  bufStart = INT_MAX; // nothing in the buffer so far
  SetPos(0);          // setup  buffer to position 0 (start)
  if (bufLen == fileLen) Close();
}

Buffer::Buffer(Buffer *b) {
  buf = b->buf;
  b->buf = NULL;
  bufStart = b->bufStart;
  bufLen = b->bufLen;
  fileLen = b->fileLen;
  pos = b->pos;
  stream = b->stream;
  b->stream = NULL;
  isUserStream = b->isUserStream;
}

Buffer::~Buffer() {
  Close();
  if (buf != NULL) {
    delete [] buf;
    buf = NULL;
  }
}

void Buffer::Close() {
  if (!isUserStream && stream != NULL) {
    fclose(stream);
    stream = NULL;
  }
}

int Buffer::Read() {
  if (pos < bufLen) {
    return buf[pos++];
  } else if (GetPos() < fileLen) {
    SetPos(GetPos()); // shift buffer start to Pos
    return buf[pos++];
  } else {
    return EoF;
  }
}

int Buffer::Peek() {
  int curPos = GetPos();
  int ch = Read();
  SetPos(curPos);
  return ch;
}

char* Buffer::GetString(int beg, int end) {
  int len = end - beg;
  char *buf = new char[len];
  int oldPos = GetPos();
  SetPos(beg);
  for (int i = 0; i < len; ++i) buf[i] = (char) Read();
  SetPos(oldPos);
  return buf;
}

int Buffer::GetPos() {
  return pos + bufStart;
}

void Buffer::SetPos(int value) {
  if (value < 0) value = 0;
  else if (value > fileLen) value = fileLen;
  if (value >= bufStart && value < bufStart + bufLen) { // already in buffer
    pos = value - bufStart;
  } else if (stream != NULL) { // must be swapped in
    fseek(stream, value, SEEK_SET);
    bufLen = (int)fread(buf, sizeof(char), MAX_BUFFER_LENGTH, stream);
    bufStart = value; pos = 0;
  } else {
    pos = fileLen - bufStart; // make Pos return fileLen
  }
}

int UTF8Buffer::Read() {
  int ch;
  do {
    ch = Buffer::Read();
    // until we find a uft8 start (0xxxxxxx or 11xxxxxx)
  } while ((ch >= 128) && ((ch & 0xC0) != 0xC0) && (ch != EOF));
  if (ch < 128 || ch == EOF) {
    // nothing to do, first 127 chars are the same in ascii and utf8
    // 0xxxxxxx or end of file character
  } else if ((ch & 0xF0) == 0xF0) {
    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    int c1 = ch & 0x07; ch = Buffer::Read();
    int c2 = ch & 0x3F; ch = Buffer::Read();
    int c3 = ch & 0x3F; ch = Buffer::Read();
    int c4 = ch & 0x3F;
    ch = (((((c1 << 6) | c2) << 6) | c3) << 6) | c4;
  } else if ((ch & 0xE0) == 0xE0) {
    // 1110xxxx 10xxxxxx 10xxxxxx
    int c1 = ch & 0x0F; ch = Buffer::Read();
    int c2 = ch & 0x3F; ch = Buffer::Read();
    int c3 = ch & 0x3F;
    ch = (((c1 << 6) | c2) << 6) | c3;
  } else if ((ch & 0xC0) == 0xC0) {
    // 110xxxxx 10xxxxxx
    int c1 = ch & 0x1F; ch = Buffer::Read();
    int c2 = ch & 0x3F;
    ch = (c1 << 6) | c2;
  }
  return ch;
}

ExScanner::ExScanner(char* fileName) {
  FILE* stream;
  char *chFileName = coco_string_create(fileName);
  if ((stream = fopen(chFileName, "rb")) == NULL) {
    MRSWARN((std::string)"ExScanner: Cannot open file %s"+fileName);
    exit(1);
  }
  coco_string_delete(chFileName);
  buffer = new Buffer(stream, false);
  Init();
}

ExScanner::ExScanner(FILE* s) {
  buffer = new Buffer(s, true);
  Init();
}

ExScanner::ExScanner(char* s, int i) {
  (void) i; // FIXME This parameter is unused
  buffer = new Buffer(s);
  Init();
}
ExScanner::ExScanner() { tval=NULL; buffer=NULL; tail=NULL; }
void ExScanner::setString(const char* s)
{
  delete [] tval;
  delete buffer;
  while (tail!=NULL) { Token* y=tail->next; delete tail; tail=y; }
  buffer = new Buffer(s);
  Init();
}

ExScanner::~ExScanner() {
  delete [] tval;
  delete buffer;
  while (tail!=NULL) { Token* y=tail->next; delete tail; tail=y; }
}

void ExScanner::Init() {
  EOL    = '\n';
  eofSym = 0;
  maxT = 61;
  noSym = 61;
  for (int i = 48; i <= 57; ++i) start.set(i, 52);
  for (int i = 46; i <= 46; ++i) start.set(i, 3);
  for (int i = 39; i <= 39; ++i) start.set(i, 4);
  for (int i = 65; i <= 90; ++i) start.set(i, 53);
  for (int i = 97; i <= 122; ++i) start.set(i, 53);
  for (int i = 47; i <= 47; ++i) start.set(i, 54);
  start.set(36, 55);
  start.set(62, 56);
  start.set(60, 57);
  start.set(43, 58);
  start.set(45, 59);
  start.set(42, 60);
  start.set(37, 61);
  start.set(38, 62);
  start.set(124, 63);
  start.set(94, 37);
  start.set(40, 38);
  start.set(41, 39);
  start.set(33, 64);
  start.set(61, 41);
  start.set(44, 45);
  start.set(123, 65);
  start.set(125, 46);
  start.set(64, 48);
  start.set(91, 49);
  start.set(93, 50);
  start.set(58, 51);
  start.set(Buffer::EoF, -1);
  keywords.set((char *)"/", 26);
  keywords.set((char *)".", 47);
  keywords.set((char *)"Stream", 51);
  keywords.set((char *)"true", 52);
  keywords.set((char *)"false", 53);
  keywords.set((char *)"map", 54);
  keywords.set((char *)"iter", 55);
  keywords.set((char *)"for", 56);
  keywords.set((char *)"rfor", 57);
  keywords.set((char *)"in", 58);
  keywords.set((char *)"use", 59);
  keywords.set((char *)"load", 60);


  tvalLength = 128;
  tval = new char[tvalLength]; // text of current token

  pos = -1; line = 1; col = 0;
  oldEols = 0;
  NextCh();
  if (ch == 0xEF) { // check optional byte order mark for UTF-8
    NextCh(); int ch1 = ch;
    NextCh(); int ch2 = ch;
    if (ch1 != 0xBB || ch2 != 0xBF) {
      MRSWARN("ExScanner: Illegal byte order mark at start of file.");
//			exit(1);
    }
    Buffer *oldBuf = buffer;
    buffer = new UTF8Buffer(buffer); col = 0;
    delete oldBuf; oldBuf = NULL;
    NextCh();
  }


  tail = pt = tokens = CreateToken(); // first token is a dummy
  pt->val=new char[1]; pt->val[0]='\0';
}

void ExScanner::NextCh() {
  if (oldEols > 0) { ch = EOL; oldEols--; }
  else {
    pos = buffer->GetPos();
    ch = buffer->Read(); col++;
    // replace isolated '\r' by '\n' in order to make
    // eol handling uniform across Windows, Unix and Mac
    if (ch == '\r' && buffer->Peek() != '\n') ch = EOL;
    if (ch == EOL) { line++; col = 0; }
  }

}

void ExScanner::AddCh() {
  if (tlen >= tvalLength) {
    tvalLength *= 2;
    char* newBuf = new char[tvalLength];
    memcpy(newBuf, tval, tlen*sizeof(char));
    delete tval;
    tval = newBuf;
  }
  tval[tlen++] = ch;
  NextCh();
}


bool ExScanner::Comment0() {
  int level = 1, pos0 = pos, line0 = line, col0 = col;
  (void) pos0; (void) col0; // FIXME Unused variables
  NextCh();
  for(;;) {
    if (ch == 10) {
      level--;
      if (level == 0) { oldEols = line - line0; NextCh(); return true; }
      NextCh();
    } else if (ch == buffer->EoF) return false;
    else NextCh();
  }
}

bool ExScanner::Comment1() {
  int level = 1, pos0 = pos, line0 = line, col0 = col;
  NextCh();
  if (ch == '*') {
    NextCh();
    for(;;) {
      if (ch == '*') {
        NextCh();
        if (ch == ')') {
          level--;
          if (level == 0) { oldEols = line - line0; NextCh(); return true; }
          NextCh();
        }
      } else if (ch == '(') {
        NextCh();
        if (ch == '*') {
          level++; NextCh();
        }
      } else if (ch == buffer->EoF) return false;
      else NextCh();
    }
  } else {
    buffer->SetPos(pos0); NextCh(); line = line0; col = col0;
  }
  return false;
}


Token* ExScanner::CreateToken() { return new Token(); }

Token* ExScanner::NextToken() {
  while ((ch == L' ') || ((ch >= 9) && (ch <= 10)) || (ch == 13))
    NextCh();
  if (((ch == '#') && (Comment0())) || ((ch == '(') && (Comment1())))
    return NextToken();
  int apx = 0;
  t = CreateToken();
  t->pos = pos; t->col = col; t->line = line;
  int state = start.state(ch);
  tlen = 0; AddCh();

  switch (state) {
  case -1: { t->kind = eofSym; break; } // NextCh already done
  case 0: { t->kind = noSym; break; }   // NextCh already done
  case 1:
case_1:
    {
      tlen -= apx;
      buffer->SetPos(t->pos); NextCh(); line = t->line; col = t->col;
      for (int i = 0; i < tlen; ++i) NextCh();
      t->kind = 1; break;
    }
  case 2:
case_2:
    if (ch >= '0' && ch <= '9') {AddCh(); goto case_2;}
    else {t->kind = 2; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 3:
case_3:
    if (ch >= '0' && ch <= '9') {AddCh(); goto case_3;}
    else {t->kind = 2; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 4:
case_4:
    if (ch == 39) {AddCh(); goto case_5;}
    else if ((ch <= '&') || ((ch >= '(') && (ch <= 65535))) {AddCh(); goto case_4;}
    else {t->kind = noSym; break;}
  case 5:
case_5:
    {t->kind = 3; break;}
  case 6:
case_6:
    {t->kind = 4; break;}
  case 7:
case_7:
    if (ch == '/') {AddCh(); goto case_8;}
    else if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || (ch == '_') || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_7;}
    else {t->kind = noSym; break;}
  case 8:
case_8:
    if (((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_9;}
    else {t->kind = noSym; break;}
  case 9:
case_9:
    if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || (ch == '_') || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_10;}
    else {t->kind = 6; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 10:
case_10:
    if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || ch == '_' || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_10;}
    else if (ch == '/') {AddCh(); goto case_11;}
    else {t->kind = 6; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 11:
case_11:
    if (((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_12;}
    else {t->kind = noSym; break;}
  case 12:
case_12:
    if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || ch == '_' || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_66;}
    else if (ch == '/') {AddCh(); goto case_11;}
    else {t->kind = 6; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 13:
case_13:
    if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || ch == '_' || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_13;}
    else if (ch == '/') {AddCh(); goto case_14;}
    else {t->kind = 6; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 14:
case_14:
    if (((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_15;}
    else {t->kind = noSym; break;}
  case 15:
case_15:
    if (((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_67;}
    else if (ch == '/') {AddCh(); goto case_14;}
    else if (((ch >= '0') && (ch <= '9')) || (ch == '_')) {AddCh(); goto case_15;}
    else {t->kind = 6; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 16:
case_16:
    {t->kind = 7; break;}
  case 17:
case_17:
    if (ch == '>') {AddCh(); goto case_18;}
    else {t->kind = noSym; break;}
  case 18:
case_18:
    {t->kind = 9; break;}
  case 19:
case_19:
    {t->kind = 10; break;}
  case 20:
case_20:
    if (ch == '>') {AddCh(); goto case_21;}
    else {t->kind = noSym; break;}
  case 21:
case_21:
    {t->kind = 11; break;}
  case 22:
case_22:
    if (ch == '>') {AddCh(); goto case_23;}
    else {t->kind = noSym; break;}
  case 23:
case_23:
    {t->kind = 12; break;}
  case 24:
case_24:
    if (ch == '>') {AddCh(); goto case_25;}
    else {t->kind = noSym; break;}
  case 25:
case_25:
    {t->kind = 13; break;}
  case 26:
case_26:
    {t->kind = 14; break;}
  case 27:
case_27:
    {t->kind = 15; break;}
  case 28:
case_28:
    {t->kind = 16; break;}
  case 29:
case_29:
    {t->kind = 17; break;}
  case 30:
case_30:
    {t->kind = 18; break;}
  case 31:
case_31:
    if (ch == '>') {AddCh(); goto case_32;}
    else {t->kind = noSym; break;}
  case 32:
case_32:
    {t->kind = 19; break;}
  case 33:
case_33:
    if (ch == '>') {AddCh(); goto case_34;}
    else {t->kind = noSym; break;}
  case 34:
case_34:
    {t->kind = 20; break;}
  case 35:
case_35:
    {t->kind = 21; break;}
  case 36:
case_36:
    {t->kind = 22; break;}
  case 37:
  {t->kind = 28; break;}
  case 38:
  {t->kind = 29; break;}
  case 39:
  {t->kind = 30; break;}
  case 40:
case_40:
    {t->kind = 32; break;}
  case 41:
  {t->kind = 34; break;}
  case 42:
case_42:
    {t->kind = 35; break;}
  case 43:
case_43:
    {t->kind = 37; break;}
  case 44:
case_44:
    {t->kind = 39; break;}
  case 45:
  {t->kind = 42; break;}
  case 46:
  {t->kind = 44; break;}
  case 47:
case_47:
    {t->kind = 45; break;}
  case 48:
  {t->kind = 46; break;}
  case 49:
  {t->kind = 48; break;}
  case 50:
  {t->kind = 49; break;}
  case 51:
  {t->kind = 50; break;}
  case 52:
case_52:
    if ((ch >= '0') && (ch <= '9')) {AddCh(); goto case_52;}
    else if (ch == '.') {apx++; AddCh(); goto case_68;}
    else {t->kind = 1; break;}
  case 53:
case_53:
    if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_53;}
    else if (ch == '/') {AddCh(); goto case_8;}
    else if (ch == '_') {AddCh(); goto case_7;}
    else {t->kind = 5; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 54:
    if (((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_13;}
    else if (ch == '>') {AddCh(); goto case_22;}
    else {t->kind = 6; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 55:
    if (ch == 'f' || ch == 't') {AddCh(); goto case_6;}
    else {t->kind = noSym; break;}
  case 56:
    if (ch == '>') {AddCh(); goto case_16;}
    else if (ch == '=') {AddCh(); goto case_43;}
    else {t->kind = 36; break;}
  case 57:
    if (ch == '<') {AddCh(); goto case_69;}
    else if (ch == '-') {AddCh(); goto case_40;}
    else if (ch == '=') {AddCh(); goto case_44;}
    else {t->kind = 38; break;}
  case 58:
    if (ch == '>') {AddCh(); goto case_17;}
    else {t->kind = 23; break;}
  case 59:
    if (ch == '>') {AddCh(); goto case_70;}
    else {t->kind = 24; break;}
  case 60:
    if (ch == '>') {AddCh(); goto case_20;}
    else {t->kind = 25; break;}
  case 61:
    if (ch == '>') {AddCh(); goto case_24;}
    else {t->kind = 27; break;}
  case 62:
    if (ch == '>') {AddCh(); goto case_31;}
    else {t->kind = 40; break;}
  case 63:
    if (ch == '>') {AddCh(); goto case_33;}
    else {t->kind = 41; break;}
  case 64:
    if (ch == '=') {AddCh(); goto case_42;}
    else {t->kind = 33; break;}
  case 65:
    if (ch == '?') {AddCh(); goto case_47;}
    else {t->kind = 43; break;}
  case 66:
case_66:
    if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || ch == '_' || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_66;}
    else if (ch == '/') {AddCh(); goto case_11;}
    else {t->kind = 6; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 67:
case_67:
    if (((ch >= '0') && (ch <= '9')) || ((ch >= 'A') && (ch <= 'Z')) || ch == '_' || ((ch >= 'a') && (ch <= 'z'))) {AddCh(); goto case_67;}
    else if (ch == '/') {AddCh(); goto case_14;}
    else {t->kind = 6; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 68:
case_68:
    if (((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z'))) {apx++; AddCh(); goto case_1;}
    else if ((ch >= '0') && (ch <= '9')) {apx = 0; AddCh(); goto case_2;}
    else {t->kind = 2; t->val = coco_string_create(tval, 0, tlen); t->kind = keywords.get(t->val, t->kind); return t;}
  case 69:
case_69:
    if (ch == '+') {AddCh(); goto case_26;}
    else if (ch == '-') {AddCh(); goto case_27;}
    else if (ch == '*') {AddCh(); goto case_28;}
    else if (ch == '/') {AddCh(); goto case_29;}
    else if (ch == '%') {AddCh(); goto case_30;}
    else if (ch == '&') {AddCh(); goto case_35;}
    else if (ch == '|') {AddCh(); goto case_36;}
    else {t->kind = 8; break;}
  case 70:
case_70:
    if (ch == '>') {AddCh(); goto case_19;}
    else {t->kind = 31; break;}

  }
  t->val=coco_string_create(tval, 0, tlen);
  return t;
}

// get the next token (possibly a token already seen during peeking)
Token* ExScanner::Scan() {
  if (tokens->next == NULL) { tokens->next=NextToken(); }
  tokens=tokens->next; pt=tokens;
  return tokens;
}

// peek for the next token, ignore pragmas
Token* ExScanner::Peek() {
  if (pt->next == NULL) {
    do {
      pt = pt->next = NextToken();
    } while (pt->kind > maxT); // skip pragmas
  } else {
    do {
      pt = pt->next;
    } while (pt->kind > maxT);
  }
  return pt;
}

// make sure that peeking starts at the current scan position
void ExScanner::ResetPeek() {
  pt = tokens;
}
