// 
// Our very own JSON parser - BUT WHY!! 
//  DayZ codebase has a JsonFileLoader<Class T> so why reinvent the wheel?
//  The problem is that because this is a templated class you are required to
//  specify all class(s) and class members in advance. Your JSON had better
//  match this or there will be trouble! This created problems if I wanted
//  change/delete things. The work around is usually to check the json file 
//  version and upgrade it accordingly. But for me this was annoying. JSON is not
//  XML, I really think it should only be used as a data store, not as a way
//  to serialize objects. Maybe I'm the only one who feels that way?
// 

const bool BBB_JSON_DEBUG = false;
const bool BBB_JSON_ENFORCE_KEY_CASE_SENSITIVE = false;

string KeyToLower(string key) {
    string copy = key + "";
    if(!BBB_JSON_ENFORCE_KEY_CASE_SENSITIVE) copy.ToLower();
    return copy;
}

void json_debug(string str)
{
    if(BBB_JSON_DEBUG) BBB_Log.Log("[JSON]" + str);
}

void json_error(string str)
{
    BBB_Log.Log("[JSON]" + str);
}

//! JSON Decode state machine states. Each state represents the next expected
//! element(s).
enum BBB_EJState
{
    //! Expecting object key or }
    KEY_CLOSEBRACE,
    //! Expecting : after key.
    COLON,
    //! Expecting { [ value after :
    OPENBRACE_OPENBRACKET_VALUE,
    //! Expecting } { [ or value after {
    CLOSEBRACKET_OPENBRACE_OPENBRACKET_VALUE,
    //! Expecting } or , after object value
    CLOSEBRACE_COMMA,
    //! Expecting } or , after array value
    CLOSEBRACKET_COMMA,
    //! Expecting ] or , after array value
    CLOSE_COMMA
    //! Expecting ] } or , after value
}

string BBB_JsonStateName(BBB_EJState state)
{
    switch(state)
    {
        case BBB_EJState.KEY_CLOSEBRACE: return "}";
        case BBB_EJState.COLON: return ":";
        case BBB_EJState.OPENBRACE_OPENBRACKET_VALUE: return "{ [ or value";
        case BBB_EJState.CLOSEBRACKET_OPENBRACE_OPENBRACKET_VALUE: return "} { [ or value";
        case BBB_EJState.CLOSEBRACE_COMMA: return "} or ,";
        case BBB_EJState.CLOSEBRACKET_COMMA: return "] or ,";
        case BBB_EJState.CLOSE_COMMA: return "} ] or ,";
    }
    return "";
}


//! A single JSON Key Value pair.
class BBB_JsonKeyValue
{
    private string m_sKey;
    private int m_iIndex;
    private string m_sValue;
    private typename m_tValue;
    private ref BBB_JsonKeyValueMap Child;
    private int m_iLineNumber;
    private bool m_bNoNewLine;
    private typename m_tParentType;

    void BBB_JsonKeyValue(string key, string value, typename type, int line, int index, typename parentType)
    {
        m_tParentType = parentType;
        if(m_tParentType==array)
        {
            // we need something for a hash table key.
            m_sKey = index.ToString();
        } else {
            m_sKey = key;
        }
        m_iIndex = index;
        
        if(type==array || type==map)
        {
            m_sValue = "";
        } else {
            m_sValue = value;
        }
        m_tValue = type;
        m_iLineNumber = line;
    }

    void SetNoNewLine() { m_bNoNewLine = true; }
    bool GetNoNewline() { return m_bNoNewLine; }
    int GetLineNumber() { return m_iLineNumber; }
    void SetChildMap(BBB_JsonKeyValueMap child) { Child = child; }
    ref BBB_JsonKeyValueMap GetChildMap() { return Child; }
    string GetKey() { return KeyToLower(m_sKey); }
    int GetIndex() { return m_iIndex; }
    string GetValue() { return m_sValue; }
    float GetValueFloat() { return m_sValue.ToFloat(); }
    int GetValueInt() { return m_sValue.ToInt(); }
    bool GetValueBool() {
        if(m_sValue=="true") return true;
        return false;
    }
    vector GetValueVector() { return m_sValue.ToVector(); }
    void SetValue(string value) { m_sValue = value; }
    typename GetType() { return m_tValue; }

    string GetJSON(bool isLast) {
        string json = "";
        if(m_tParentType==map)
        {
            json += "\"" + m_sKey + "\": ";
        }

        if(m_tValue==map) {
            json += "{";
        } else if(m_tValue==array) {
            json += "[";
        } else if(m_tValue==string || m_tValue==bool) {
            json += "\"" + m_sValue + "\"";
        } else if(m_tValue==void) {
            json += "\"null\"";
        } else {
            json += m_sValue;
        }

        if(!isLast && m_tValue!=map && m_tValue!=array) json += ", ";

        return json;
    }
}

typedef map<string, ref BBB_JsonKeyValue> BBB_JsonKeyValueMap 


class BBB_Json
{

    private ref BBB_JsonKeyValueMap docRoot;
    private ref array<BBB_JsonKeyValueMap> docStack;
    private ref array<BBB_JsonKeyValue> containerStack;

    private int lineNum = 0;
    private string token;
    private int tokenType;
    private bool error = false;
    private string errorMessage = "";
    private string key;
    private int state = 0;
    private ref BBB_JsonKeyValueMap activeNode;
    private BBB_JsonKeyValue activeContainer;

    void BBB_Json ()
    {
        docRoot = new BBB_JsonKeyValueMap();
        docStack = new array<BBB_JsonKeyValueMap>;
        containerStack = new array<BBB_JsonKeyValue>;
    }

    BBB_JsonKeyValueMap GetDocRoot()
    {
        return docRoot;
    }

    bool getError() {return error;}
    string getErrorMessage() {return errorMessage;}
    private void resetError() {
        error = false;
        errorMessage = "";
    }

    private void GetKeysSortedByLineNum(BBB_JsonKeyValueMap vm, out TStringArray keys)
    { 
        foreach(string k, ref BBB_JsonKeyValue value: vm)
        {
            keys.Insert(value.GetLineNumber().ToStringLen(6) + value.GetIndex().ToStringLen(6) + value.GetKey());
        }
        //PluginDeveloper.SortStringArray()
        keys.Sort();
        for(int a=0; a<keys.Count(); a++)
        {
            int l = keys.Get(a).Length();
            //json_debug(l + " " + keys.Get(a));
            keys.Set(a, keys.Get(a).Substring(12, keys.Get(a).Length() - 12) );
        }
    }

    void dump(BBB_JsonKeyValueMap vm, int indent)
    {
        resetError();

        if(vm==null) vm = docRoot;
        string pad;
        for(int a=0; a < indent; a++)
        {
            pad += " ";
        }
        json_debug(pad + " cnt=" + vm.Count() + " vm=" + vm);
        if(indent>24) return;
        

        TStringArray keys = new TStringArray();
        GetKeysSortedByLineNum(vm, keys);
        //json_debug("KEYS " + keys);
        

        foreach(string k: keys)
        {
            BBB_JsonKeyValue value = vm.Get(k);
            k = Descape(k);

            if(value.GetType()==map) {
                json_debug(pad + value.GetType() + " " + k + " {");
                dump(value.GetChildMap(), indent+4);
            } else if(value.GetType()==array) {
                json_debug(pad + value.GetType() + " " + k + " [");
                dump(value.GetChildMap(), indent+4);
            } else if(value.GetType()==string) {
                json_debug(pad + value.GetType() + " " + k + "=" + Descape(value.GetValue()));
            } else {
                json_debug(pad + value.GetType() + " " + k + "=" + value.GetValue());
                /*if(value.GetType()==float) {
                    json_debug("** " + value.GetValue().GetValueFloat());
                }*/
            }
            
        }
    }

    string Escape(string str)
    {
        return str;
    }

    string Descape(string str)
    {
        string outString = "";
        int parseState = 0; // seek
        int start = 0;
        int digitCount = 0;
        int digitSum = 0;
        for(int a=0; a < str.Length(); a++)
        {
            if(parseState==0) {
                int b = str.IndexOfFrom(a, "\\");
                //json_debug("descape_a [" + str + "] [" + outString + "] " + a + " " + b);
                
                if(b==-1)
                {
                    outString += str.Substring(start, str.Length() - start);
                    return outString;
                }

                a=b;
                if(a - start > 0) {
                    //json_debug("descape_c " + start + " " + (a - start - 1));
                    outString += str.Substring(start, a - start);
                }

                parseState = 1;
                start = b+1;
                
            } else if(parseState==1) {
                start++;
                string char = str.Get(a);
                //json_debug("descape_b [" + str + "] [" + outString + "] [" + char + "]" + a + " " + start);
                if(char=="/") { outString += "/"; }
                else if (char=="\"") { outString += "\"" ;}
                else if (char=="b") { outString += (8).AsciiToString();}
                else if (char=="f") { outString += (12).AsciiToString();}
                else if (char=="n") { outString += (10).AsciiToString();}
                else if (char=="r") { outString += (13).AsciiToString();}
                else if (char=="t") { outString += (9).AsciiToString();}
                else if (char=="u") { parseState=2; continue;}
                else {
                    return "!Bad Escape";
                }
                parseState = 0;
            } else if (parseState==2) {
                start++;
                int digit = str.Get(a).ToAscii();
                if(digit >= 48 && digit <= 57) {
                    digitSum *= 10;
                    digitSum += digit - 48;
                    digitCount++;
                    
                    if(digitCount==4)
                    {
                        parseState = 0;
                        outString += digitSum.AsciiToString();
                        //json_debug("descape_y [" + digitSum + "] [" + outString + "]");
                        digitSum = 0; digitCount = 0;
                    }
                } else {
                    return "!Bad Digits";
                    //digitSum = 0; digitCount = 0;
                    //parseState = 0;
                }
            }
        }
        return outString;
    }
    
    typename whatType(string tStr, int tType)
    {
        if(tType==1 && tStr=="{") return map;
        if(tType==1 && tStr=="[") return array;
        if(tType==2) {
            return string;
        }
        if(tType==3) {
            if(tStr=="false" || tStr=="true") return bool;
            if(tStr=="null") return void;
            return string;
        }
        if(tType==4) {
            if(tStr.Contains("."))
            {
                return float;
            } else {
                return  int;
            }
        }
        return Class;
    }

    void save_dump(out string data, BBB_JsonKeyValueMap vm, int indent, out int lineNo)
    {
        
        resetError();

        if(vm==null || vm.Count()==0) return;
        if(indent>100) return;

        string pad;
        for(int a=0; a < indent; a++)
        {
            // padding applied at start of any new line.
            pad += " ";
        }

        TStringArray keys = new TStringArray();
        GetKeysSortedByLineNum(vm, keys);
        //json_debug("KEYS " + keys);
        string last_key = keys.Get(keys.Count() - 1);
        string line = "";
        foreach(string k: keys)
        {
            BBB_JsonKeyValue value = vm.Get(k);
            if(lineNo==0 || lineNo!=value.GetLineNumber()) 
            {
                lineNo = value.GetLineNumber();
                data += "\n" + pad;
            }

            //GetLineNumber
            data += value.GetJSON( last_key==k );

            if(value.GetType()==map) {
                save_dump(data, value.GetChildMap(), indent+4, lineNo);
                if(!value.GetNoNewline()) data += "\n" + pad;
                data += "}";
                if(last_key!=k) data+=", ";

            } else if(value.GetType()==array) {
                save_dump(data, value.GetChildMap(), indent+4, lineNo);
                if(!value.GetNoNewline()) data += "\n" + pad;
                data += "]";
                if(last_key!=k) data+=", ";
            }

            

        }
    }

    void save(string filename)
    {
        resetError();

        json_debug("Save " + filename);
        lineNum = 0;
        string data = "{";
        save_dump(data, docRoot, 4, lineNum);
        data += "\n}";

        FileHandle fh = OpenFile(filename, FileMode.WRITE);
        if (fh) {
            FPrint(fh, data);
            CloseFile(fh);
        } else {
            json_debug("Could not access " + filename);
        }
        
    }



    private void Push(BBB_JsonKeyValue newNode)
    {
        // if array or map, push it on stack and set as active.
        docStack.Insert(activeNode);
        containerStack.Insert(activeContainer);
        activeContainer = newNode;
        activeNode = new BBB_JsonKeyValueMap();
        newNode.SetChildMap(activeNode);
        
        json_debug(lineNum.ToString() + ": PUSH " + containerStack.Get(containerStack.Count() - 1) + " " + docStack.Get(docStack.Count() - 1) + " | " + containerStack);
    }

    private void Pop()
    {
        json_debug(lineNum.ToString() + ": POP " + containerStack.Get(containerStack.Count() - 1) + " " + docStack.Get(docStack.Count() - 1) + " | " + containerStack);
        if(docStack.Count() > 0) {

            activeNode = docStack.Get(docStack.Count() - 1);
            
            json_debug(lineNum.ToString() + " SetNoNewLine ? " + activeContainer.GetLineNumber() );
            if(activeContainer.GetLineNumber() == lineNum) 
            {
                activeContainer.SetNoNewLine();
            }

            activeContainer = containerStack.Get(containerStack.Count() - 1);

            if(activeContainer)
            {
                if(activeContainer.GetType()==array)
                {
                    //json_debug("Parent is Array");
                    state=BBB_EJState.CLOSE_COMMA;  // wait ] } ,
                } else {
                    //json_debug("is Object");
                    state=BBB_EJState.CLOSEBRACE_COMMA;    // wait } ,
                }
            }

            docStack.Remove(docStack.Count() - 1);
            containerStack.Remove(containerStack.Count() - 1);

        } else {
            activeNode = docRoot;
            activeContainer = null;
            state = 5;
        }
        //json_debug(lineNum.ToString() + ": Active Parent=" + activeContainer);
        //json_debug(lineNum.ToString() + ": cnt=" + activeNode.Count() + " keys=" + activeNode.GetKeyArray());                        
    }

    BBB_JsonKeyValueMap load(string filename)
    {
        resetError();
        FileHandle file_handle = OpenFile(filename, FileMode.READ);
        
        string line;
        error = false;
        lineNum = 0;
        key = "";
        state = BBB_EJState.KEY_CLOSEBRACE;
        activeNode = null;
        activeContainer = null;

        while ( FGets( file_handle, line ) > 0 )
        {
            lineNum++;
            while( !error )
            {
                typename newType;
                BBB_JsonKeyValue newNode = null;
                /* tokenTypes
                    0 - error, no token
                    1 - defined token (special characters etc. . / * )
                    2 - quoted string. Quotes are removed -> TODO
                    3 - alphabetic string
                    4 - number
                    5 - end of line -> TODO
                */
                tokenType = line.ParseStringEx(token);
                if(tokenType==0 || tokenType==5) break;

                json_debug(lineNum.ToString() + ": state=" + state + " " + tokenType.ToString() + ":" + token + " activeNode " + activeNode);
                if(activeNode==null) {
                    // first token must be { our doc root.
                    if(tokenType==1 && token=="{") {
                        activeNode = docRoot;
                    } else {
                        errorMessage = "line " + lineNum.ToString() + ": Error expecting { to be first token";
                        error = true;
                        break;
                    }
                } else if(state==BBB_EJState.KEY_CLOSEBRACE) {
                    // We're expecting a string key or } if this is an empty object
                    if(tokenType==2) {
                        key = token;
                        state = BBB_EJState.COLON;
                    } else if(tokenType==1 && token=="}") {
                        Pop();
                        continue;
                    } else {
                        errorMessage = "line " + lineNum.ToString() + ": Expecting key but got " + token + " type=" + tokenType;
                        error = true;
                        break;                      
                    }
                } else if(state==BBB_EJState.COLON) {   // :
                    // expecting : between 
                    if(tokenType==1 && token==":") {
                        state = BBB_EJState.OPENBRACE_OPENBRACKET_VALUE;
                    } else {
                        errorMessage = "line " + lineNum.ToString() + ": Expecting : but got " + token + " type=" + tokenType;
                        error = true;
                        break;                              
                    }

                } else if(state==BBB_EJState.OPENBRACE_OPENBRACKET_VALUE) {   // value

                    // handle key/value pair.
                    if(tokenType==1 && token=="{") {
                        // start new child map type.
                        state = BBB_EJState.KEY_CLOSEBRACE;
                    } else if(tokenType==1 && token=="[") {
                        // start new child array type.
                        state = BBB_EJState.CLOSEBRACKET_OPENBRACE_OPENBRACKET_VALUE;
                    } else if(tokenType==2 || tokenType==3 || tokenType==4) {
                        // push string or number
                        state = BBB_EJState.CLOSEBRACE_COMMA;
                    } else {
                        errorMessage = "line " + lineNum.ToString() + ": Expecting value but got " + token + " type=" + tokenType;
                        error = true;
                        break;
                    }

                    newType = whatType(token, tokenType);
                    //json_debug(lineNum.ToString() + ": " + key + "=" + token);
                    newNode = new BBB_JsonKeyValue(key, token, newType, lineNum, activeNode.Count(), map);
                    if(activeNode.Contains(newNode.GetKey()))
                    {
                       errorMessage = "line " + lineNum.ToString() + ": replacing duplicate key \"" + newNode.GetKey() + "\""; 
                       activeNode.Remove(newNode.GetKey());
                    }

                    activeNode.Insert(newNode.GetKey(), newNode);
                    if(tokenType==1) Push(newNode);

                } else if(state==BBB_EJState.CLOSEBRACKET_OPENBRACE_OPENBRACKET_VALUE) {

                    if(tokenType==1 && token=="]") {
                        Pop();
                        continue;

                    } else if(tokenType==1 && token=="{") {
                        // start new child map type.
                        state = BBB_EJState.KEY_CLOSEBRACE;
                    } else if(tokenType==1 && token=="[") {
                        // start new child array type.
                        state = BBB_EJState.CLOSEBRACKET_OPENBRACE_OPENBRACKET_VALUE;
                    } else if(tokenType==2 || tokenType==4) {
                        // push string or number
                        state = BBB_EJState.CLOSEBRACKET_COMMA;
                    } else {
                        errorMessage = "line " + lineNum.ToString() + ": Expecting value but got " + token + " type=" + tokenType;
                        error = true;
                        break;   
                    }

                    newType = whatType(token, tokenType);
                    // Arrays and array elements have not key
                    newNode = new BBB_JsonKeyValue("", token, newType, lineNum, activeNode.Count(), array);
                    activeNode.Insert(newNode.GetKey(), newNode);
                    if(tokenType==1) Push(newNode);
                    
                } else if(state==BBB_EJState.CLOSE_COMMA) {
                    if(tokenType==1 && (token=="]" || token=="}")) {
                        Pop();
                        continue;

                    } else if(tokenType==1 && token==",") {
                        state = BBB_EJState.CLOSEBRACKET_OPENBRACE_OPENBRACKET_VALUE;
                    } else {
                        errorMessage = "line " + lineNum.ToString() + ": Expecting , ] } but got " + token + " type=" + tokenType;
                        error = true;
                        break;
                    }
                } else if(state==BBB_EJState.CLOSEBRACKET_COMMA) {
                    if(tokenType==1 && token=="]") {
                        Pop();
                        continue;

                    } else if(tokenType==1 && token==",") {
                        state = BBB_EJState.CLOSEBRACKET_OPENBRACE_OPENBRACKET_VALUE;
                    } else {
                        errorMessage = "line " + lineNum.ToString() + ": Expecting ] , but got " + token + " type=" + tokenType;
                        error = true;
                        break;
                    }
                } else if(state==BBB_EJState.CLOSEBRACE_COMMA) {
                    if(tokenType==1 && token=="}") {
                        Pop();
                        continue;
                    } else if(tokenType==1 && token==",") {
                        state = BBB_EJState.KEY_CLOSEBRACE;
                    } else {
                        errorMessage = "line " + lineNum.ToString() + ": Expecting , } but got " + token + " type=" + tokenType;
                        error = true;
                        break;
                    }
                } else {
                    errorMessage = "line " + lineNum.ToString() + ": Bad state? got " + token + " type=" + tokenType;
                    error = true;
                    break; 
                }
                
            }
        }
        
        if (error) {
            json_error(errorMessage);
        }

        CloseFile(file_handle);
        return docRoot;
    }
}


// ----------------------------------------------------------------------------
// JSON to Enforce mapping helpers.

class BBB_JsonMapMember
{
    Class m_cSelf;
    string m_sMethod;
    string m_sJsonKey;
    typename m_tCType;  // The container string, array.
    typename m_tOtype;  // The Of Type, what the container contains.

    bool m_bPopulated;

    private bool isTypePrimitive(typename type)
    {
        if(type==int || type==float || type==string || type==bool || type==vector || type==void)
        {
            return true;
        }
        return false;
    }

    private string floatToString(float fv)
    {        
        string value = fv.ToString();
        if(fv==0) value = "0.0";
        return value;
    }

    private string boolToString(bool bv)
    {
        if(bv) return "true";
        return "false";
    }

    bool Export(BBB_JsonKeyValueMap pActiveNode, BBB_JsonKeyValue parentKeyValue, bool Create)
	{
        bool empty = true;
        BBB_JsonKeyValueMap activeNode;
        BBB_JsonKeyValue KeyValue = pActiveNode.Get(m_sJsonKey);
        int result;
        string value;
        bool created = false;
        json_debug(m_sMethod + " k=" + m_sJsonKey + " jk=" + KeyValue + " c=" + Create + " >> " + pActiveNode);

        if(KeyValue==null && Create && pActiveNode)
        {
            typename parentType = map;
            if(parentKeyValue)
            {
                parentType = parentKeyValue.GetType();
            }
            // TODO: Create KeyValue and add to pActiveNode
            //void BBB_JsonKeyValue(string key, string value, typename type, int line, int index, typename parentType)
            if(m_tCType == vector)
            {
                KeyValue = new BBB_JsonKeyValue(m_sJsonKey, "", array, 0, pActiveNode.Count(), parentType);
            } else if (m_tOtype==void && !isTypePrimitive(m_tCType))
            {
                KeyValue = new BBB_JsonKeyValue(m_sJsonKey, "", map, 0, pActiveNode.Count(), parentType);
            } else {
                KeyValue = new BBB_JsonKeyValue(m_sJsonKey, "", m_tCType, 0, pActiveNode.Count(), parentType);
            }

            pActiveNode.Insert(KeyValue.GetKey(), KeyValue);
            created = true;
        }
        //json_debug("Export ...");
        if(m_tCType == vector || m_tCType == array || m_tCType == map || (m_tOtype==void && !isTypePrimitive(m_tCType)))
        {
            if(m_tCType == map || (m_tOtype==void && !isTypePrimitive(m_tCType)))
            {
                KeyValue.SetValue("{");
                json_debug("    Export2 {");

            } else {
                KeyValue.SetValue("[");
                json_debug("    Export2 [");
            }
            activeNode = KeyValue.GetChildMap();
            if(!activeNode) {
                json_debug("    Export3 create BBB_JsonKeyValueMap()");
                activeNode = new BBB_JsonKeyValueMap();
                KeyValue.SetChildMap(activeNode);
                created = true;
            } else {
                //activeNode.Clear();
            }
        }

        json_debug("    Export " + m_tCType + "<" + m_tOtype + "> " + m_sMethod + " >> " + m_sJsonKey);

        if(m_tCType == int)
        {
            int iv;
            result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, iv);
            json_debug("    Export value " + m_sMethod + "=" + iv);
            KeyValue.SetValue(iv.ToString());
            if(iv!=0) empty = false;

        } else if(m_tCType == float)
        {
            float fv;
            result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, fv);
            json_debug("    Export value " + m_sMethod + "=" + fv);
            KeyValue.SetValue(floatToString(fv));
            if(fv!=0) empty = false;

        } else if(m_tCType == string)
        {
            string sv;
            result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, sv);
            json_debug("    Export value " + m_sMethod + "=" + sv);
            KeyValue.SetValue(sv);
            if(sv!="") empty = false;

        } else if(m_tCType == bool)
        {
            bool bv;
            result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, bv);
            json_debug("    Export value " + m_sMethod + "=" + bv);
            KeyValue.SetValue(boolToString(bv));
            if(bv) empty = false;

        } else if(m_tCType == vector)
        {
            vector vv;
            result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, vv);
            json_debug("    Export value " + m_sMethod + "=" + vv);
            /*
            It's not our fault.
            float tre = 3710.810059;
            json_debug("Export tre=" + tre);
            json_debug("Export tre=" + floatToString(tre));
            json_debug("Export tre=" + tre.ToString());
            json_debug("Export tre=" + tre);
            json_debug("Export tre=" + tre.ToString());
            */
            if(vv != vector.Zero) empty = false;

            if(KeyValue.GetType()==string) {
                KeyValue.SetValue(floatToString(vv[0]) + " " + floatToString(vv[1]) + " " + floatToString(vv[2]));
            } else if (KeyValue.GetType()==array) {
                if(activeNode.Count()==3)
                {
                    activeNode.Get("0").SetValue(floatToString(vv[0]));
                    activeNode.Get("1").SetValue(floatToString(vv[1]));
                    activeNode.Get("2").SetValue(floatToString(vv[2]));
                } else {
                    // Created must be true here.
                    BBB_JsonKeyValue childKeyValue;
                    childKeyValue = new BBB_JsonKeyValue("", floatToString(vv[0]), float, 0, 0, array);
                    activeNode.Insert(childKeyValue.GetKey(), childKeyValue);
                    childKeyValue = new BBB_JsonKeyValue("", floatToString(vv[1]), float, 0, 1, array);
                    activeNode.Insert(childKeyValue.GetKey(), childKeyValue);
                    childKeyValue = new BBB_JsonKeyValue("", floatToString(vv[2]), float, 0, 2, array);
                    activeNode.Insert(childKeyValue.GetKey(), childKeyValue);
                }
            }
            //json_debug("Export v=" + KeyValue.GetValue());

        } else if(m_tOtype==void) {

            // Create an instance of our m_tOtype class we want to insert.
            Object newObj;
            result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, newObj);
            // json_debug("Export create BBB_JsonKeyValueMap()");
            // Call JSON_Export on it.
            BBB_JsonMap child;
            Class.CastTo(child,newObj);

            //child.JSON_Export(activeNode, KeyValue, Create);
            /*
            foreach(string k, BBB_JsonKeyValue v: activeNode)
            {
                json_debug("    v key=" + v.GetKey() + " value=" + v.GetValue());
            }
            */

            empty = child.JSON_Export(activeNode, KeyValue, Create);

            json_debug("    Export o " + m_sMethod + " array<" + m_tOtype + "> = " + newObj);

        } else if(m_tCType==array && KeyValue.GetType()==array) {
            json_debug("    Export o " + m_sMethod + " array<" + m_tCType + ">...");

            // TODO: Fetch line numbers of old elements to try and replicate in 
            // new array...
            // TODO: Fetch vector format before deleting "" or [] and replicate.

            BBB_JsonKeyValue newNode;
            

            if(m_tOtype==int)
            {
                array<ref int> intArray;
                result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, intArray);
                foreach(int ai: intArray)
                {
                    newNode = new BBB_JsonKeyValue("", ai.ToString(), int, 0, activeNode.Count(), array);
                    activeNode.Insert(newNode.GetKey(), newNode);
                    empty = false;
                }

                json_debug("    Export ai " + m_sMethod + " array<" + m_tOtype + ">.insert()=" + intArray);

            } else if(m_tOtype==float) {

                array<ref float> floatArray;
                result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, floatArray);
                foreach(float af: floatArray)
                {
                    newNode = new BBB_JsonKeyValue("", floatToString(af), float, 0, activeNode.Count(), array);
                    activeNode.Insert(newNode.GetKey(), newNode);
                    empty = false;
                }
                json_debug("    Export af " + m_sMethod + " array<" + m_tOtype + ">.insert()=" + floatArray);

            } else if(m_tOtype==string) {

                array<ref string> stringArray;
                result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, stringArray);
                foreach(string as: stringArray)
                {
                    newNode = new BBB_JsonKeyValue("", as, string, 0, activeNode.Count(), array);
                    activeNode.Insert(newNode.GetKey(), newNode);
                    empty = false;
                }
                json_debug("    Export as " + m_sMethod + " array<" + m_tOtype + ">.insert()=" + stringArray);

            } else if(m_tOtype==bool) {

                array<ref bool> boolArray;
                result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, boolArray);
                
                foreach(bool ab: boolArray)
                {
                    newNode = new BBB_JsonKeyValue("", boolToString(ab), bool, 0, activeNode.Count(), array);
                    activeNode.Insert(newNode.GetKey(), newNode);
                    empty = false;
                }
                json_debug("    Export ab " + m_sMethod + " array<" + m_tOtype + ">.insert()=" + boolArray);

            } else if(m_tOtype==vector) {

                array<ref vector> vectorArray;
                result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, vectorArray);
                
                foreach(vector av: vectorArray)
                {
                    string vectorJSON = floatToString(av[0]) + " " + floatToString(av[1]) + " " + floatToString(av[2]);
                    newNode = new BBB_JsonKeyValue("", vectorJSON, vector, 0, activeNode.Count(), array);
                    activeNode.Insert(newNode.GetKey(), newNode);
                    empty = false;
                }
                json_debug("    Export ab " + m_sMethod + " array<" + m_tOtype + ">.insert()=" + vectorArray);

            } else {

                array<ref BBB_BarrelLocation> arrayPointer;
                BBB_BarrelLocation obj;
                result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, arrayPointer);
                json_debug("    Export o result cnt=" + arrayPointer.Count() + " result=" + result + " " + m_cSelf + " " + m_sMethod + " " + obj);
                //Class.CastTo(ap2, arrayPointer);
                int idx = 0;
                while(1)
                {
                    obj = arrayPointer.Get(idx);
                    if(obj==null) break;
                    json_debug("    Export ao result " + idx + " " + result + " " + m_cSelf + " " + m_sMethod + " " + obj);
                    idx += 1;
                    newNode = new BBB_JsonKeyValue("", "{", map , 0, activeNode.Count(), array);
                    activeNode.Insert(newNode.GetKey(), newNode);
                    newNode.SetChildMap(new BBB_JsonKeyValueMap());
                    //BBB_JsonMap ac;
                    //Class.CastTo(ac,obj);
                    if(obj.JSON_Export(newNode.GetChildMap(), newNode, Create)==false)
                    {
                        empty = false;
                    }
                    json_debug("    HERE");
                }
            }
        }

        if(created && empty)
        {
            json_debug("    EMPTY key " + KeyValue.GetKey());
            pActiveNode.Remove(KeyValue.GetKey());
        }

        return empty;
    }

    void Import(BBB_JsonKeyValue KeyValue)
	{
        BBB_JsonKeyValueMap activeNode;
        int result;
        json_debug("key " + m_sJsonKey);
        if(m_tCType==int && KeyValue.GetType()==int) //Type
        {
            result = EnScript.SetClassVar(m_cSelf, m_sMethod, 0, KeyValue.GetValueInt());
			json_debug("Dynamic (" + result + ") " + m_tCType + " " + m_sJsonKey + "=" + KeyValue.GetValueInt());
        }
		else if(m_tCType==float && KeyValue.GetType()==float) //Type
        {
            result = EnScript.SetClassVar(m_cSelf, m_sMethod, 0, KeyValue.GetValueFloat());
			json_debug("Dynamic (" + result + ") " + m_tCType + " " + m_sJsonKey + "=" + KeyValue.GetValueFloat());
        }
		else if(m_tCType==string && KeyValue.GetType()==string) //Type
        {
            result = EnScript.SetClassVar(m_cSelf, m_sMethod, 0, KeyValue.GetValue());
			json_debug("Dynamic (" + result + ") " + m_tCType + " " + m_sJsonKey + "=" + KeyValue.GetValue());
        } else if(m_tCType==bool) {
            result = EnScript.SetClassVar(m_cSelf, m_sMethod, 0, KeyValue.GetValueBool());
			json_debug("Dynamic (" + result + ") " + m_tCType + " " + m_sJsonKey + "=" + KeyValue.GetValue());
        
        }
        else if(m_tCType==vector)
        {
            // Enforce vectors can be given as an array of 3 floats or a string
            // of space seperated floats. So we support both methods in the 
            // JSON files. String case is simplest. 
            if(KeyValue.GetType()==string) {
                result = EnScript.SetClassVar(m_cSelf, m_sMethod, 0, KeyValue.GetValueVector());
                json_debug("Dynamic (" + result + ") " + m_tCType + " " + m_sJsonKey + "=" + KeyValue.GetValueVector());
            } else if(KeyValue.GetType()==array) {
                activeNode = KeyValue.GetChildMap();
                if(activeNode.Count()==3)
                {
                    json_debug("Dynamic vector[0]=" + activeNode.Get("0").GetValueFloat());
                    json_debug("Dynamic vector[1]=" + activeNode.Get("1").GetValueFloat());
                    json_debug("Dynamic vector[2]=" + activeNode.Get("2").GetValueFloat());
                    vector vectorVector;
                    vectorVector[0] = activeNode.Get("0").GetValueFloat();
                    vectorVector[1] = activeNode.Get("1").GetValueFloat();
                    vectorVector[2] = activeNode.Get("2").GetValueFloat();
                    result = EnScript.SetClassVar(m_cSelf, m_sMethod, 0, vectorVector);

                    
                    vector v;
                    result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, v);
                    json_debug("Dynamic vector " + m_sJsonKey + "=" + v);
                }
            }
        } 
        else if(m_tOtype==void) 
        {
            // Create an instance of our m_tCType class we want to insert.
            ref BBB_JsonMap c;
            Class.CastTo(c,m_tCType.Spawn());
            c.JSON_Import(KeyValue.GetChildMap());

            result = EnScript.SetClassVar(m_cSelf, m_sMethod, 0, c);

            json_debug("Dynamic 3 " + m_sMethod + " <" + m_tCType + "> = " + c);

        }    
		else if(m_tCType==array && KeyValue.GetType()==array) //Type
		{
            // Get pointer to the array.
            // Get pointer to our json array objects.
            activeNode = KeyValue.GetChildMap();
            foreach(string subKey, ref BBB_JsonKeyValue subKeyValue: activeNode)
            {

                // Pass the child map
                if(m_tOtype==int) 
                {
                    array<int> intArray;
                    result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, intArray);
                    intArray.Insert(subKeyValue.GetValueInt());
                    json_debug("Dynamic " + m_sMethod + " array<" + m_tOtype + ">.insert()=" + intArray);

                } else if (m_tOtype==float) {
                    array<float> floatArray;
                    result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, floatArray);
                    floatArray.Insert(subKeyValue.GetValueFloat());
                    json_debug("Dynamic " + m_sMethod + " array<" + m_tOtype + ">.insert()=" + floatArray);
                } else if (m_tOtype==string) {
                    array<string> stringArray;
                    result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, stringArray);
                    stringArray.Insert(subKeyValue.GetValue());
                    json_debug("Dynamic " + m_sMethod + " array<" + m_tOtype + ">.insert()=" + stringArray);
                } else if (m_tOtype==vector) {
                    array<vector> vectorArray;
                    result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, vectorArray);
                    
                    if(KeyValue.GetType()==string) {
                        vectorArray.Insert(subKeyValue.GetValueVector());

                    } else if(KeyValue.GetType()==array) {
                        activeNode = KeyValue.GetChildMap();
                        if(activeNode.Count()==3) {
                            vector subVectorVector;
                            subVectorVector[0] = activeNode.Get("0").GetValueFloat();
                            subVectorVector[1] = activeNode.Get("1").GetValueFloat();
                            subVectorVector[2] = activeNode.Get("2").GetValueFloat();
                            vectorArray.Insert(subVectorVector);
                        }
                    }
                    json_debug("Dynamic v " + m_sMethod + " array<" + m_tOtype + ">.insert()=" + vectorArray);
                    
                } else {
                    ref array<Object> objectArray;
                    result = EnScript.GetClassVar(m_cSelf, m_sMethod, 0, objectArray);
        			
                    // Create an instance of our m_tOtype class we want to insert.
                    Object ao = m_tOtype.Spawn();
                    BBB_JsonMap ac;
                    //Class.CastTo(ao, m_tOtype.Spawn());
                    json_debug("Dynamic o ao=" + ao);
                    Class.CastTo(ac, ao);
                    json_debug("Dynamic o ac=" + ac + " ao=" + ao);
                    ac.JSON_Import(subKeyValue.GetChildMap());
                    objectArray.Insert(ao);

                    json_debug("Dynamic o " + m_sMethod + " array<" + m_tOtype + ">.insert()=" + objectArray);

                }
            }
		} 
        else if(m_tCType==map) 
        {
            // TODO:
        }
        else {
            json_debug("Unsupported type combo " + m_tCType + "<" + m_tOtype + "> " + m_sJsonKey);
        }
	}

    void BBB_JsonMapMember(Class self, string var, string jsonkey, typename type, typename oftype)
    {
        json_debug("BBB_JsonMapMember " + self + " " + var + " " + jsonkey + " " + type + " " + oftype);

        m_cSelf = self;
        m_sMethod = var;
        m_sJsonKey = jsonkey;
        m_tCType = type;
        m_tOtype = oftype;

        m_bPopulated = false;

    }
}

class BBB_JsonMap
{ 
    private ref map<string, ref BBB_JsonMapMember> m_mJSON_Members;
    
    void BBB_JsonMap()
    {
        json_debug("BBB_JsonMap()");
        m_mJSON_Members = new map<string, ref BBB_JsonMapMember>;
    }

    void JSON_Import(BBB_JsonKeyValueMap activeNode)
    {
        json_debug("-------------------------------");
        json_debug("Import " + m_mJSON_Members + " cnt=" + m_mJSON_Members.Count());
        foreach(string k, BBB_JsonMapMember v: m_mJSON_Members)
        {
             // TODO: optional case insensitivity
            BBB_JsonKeyValue KeyValue = activeNode.Get(k);
            if(KeyValue) v.Import(KeyValue);
        }
    }

    bool JSON_Export(BBB_JsonKeyValueMap activeNode, BBB_JsonKeyValue parentKeyValue, bool Create)
    {	
        json_debug("-------------------------------");
        json_debug("Export " + m_mJSON_Members + " cnt=" + m_mJSON_Members.Count());

        bool empty = true;

        foreach(string k, BBB_JsonMapMember v: m_mJSON_Members)
        {
            if(v.Export(activeNode, parentKeyValue, Create)==false) empty=false;
        }
        return empty;
    }

    void JSON_MapAdd(Class self, string var, string jsonkey, typename type, typename oftype)
    {
        BBB_JsonMapMember value = new BBB_JsonMapMember(self, var, jsonkey, type, oftype);
        m_mJSON_Members.Set(KeyToLower(value.m_sJsonKey), value);
    }

    ref BBB_JsonMapMember JSON_MapGet(string JSONkey)
    {
        return m_mJSON_Members.Get(KeyToLower(JSONkey));
    }

}