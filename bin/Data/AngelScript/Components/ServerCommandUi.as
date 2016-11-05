class ServerCommandUi : ScriptObject
{
    protected IntVector2 lastScreenSize;
    protected Array <String> chatHistory_;
    protected Node @cameraNode_;
    
    protected XMLFile @styles_;
    protected UIElement @rootElement_;
    protected Text @chatHistoryUI_;
    
    protected Button @sendMessageButton_;
    protected Text @sendMessageButtonText_;
    protected LineEdit @messageEdit_;
    protected Array <Text @> otherPlayersLabels_;
    
    protected int PROPERTY_OTHER_PLAYERS_LABELS_POOL_SIZE = 40;
    protected int PROPERTY_KEY_SWITCH_INPUT_CHAT_MESSAGE = KEY_TAB;
    protected int PROPERTY_KEY_SEND_CHAT_MESSAGE = KEY_RETURN;

    protected int PROPERTY_KEY_SPEED_UP = KEY_SHIFT;
    protected int PROPERTY_KEY_LEFT = KEY_A;
    protected int PROPERTY_KEY_RIGHT = KEY_D;
    protected int PROPERTY_KEY_FORWARD = KEY_W;
    protected int PROPERTY_KEY_BACK = KEY_S;
    protected int PROPERTY_KEY_UP = KEY_Q;
    protected int PROPERTY_KEY_DOWN = KEY_E;

    protected float PROPERTY_DEFAULT_CAMERA_MOVE_SPEED = 10.0f;
    protected float PROPERTY_SPEED_UP_MODIFER = 3.0f;
    
    ServerCommandUi ()
    {
        
    }
    
    ~ServerCommandUi ()
    {
        
    }
    
    void Start ()
    {
        cameraNode_ = scene.GetChild ("server_local_camera");
        styles_ = cache.GetResource ("XMLFile", "UI/DefaultStyle.xml");
        rootElement_ = ui.root.CreateChild ("UIElement", "server_command_ui_root");
        
        chatHistory_.Clear ();
        chatHistoryUI_ = rootElement_.CreateChild ("Text", "chat");
        chatHistoryUI_.text = "No chat messages yet...";
        chatHistoryUI_.SetStyleAuto (styles_);
            
        messageEdit_ = rootElement_.CreateChild ("LineEdit", "message_edit");
        messageEdit_.SetStyleAuto (styles_);
        messageEdit_.textElement.SetAlignment (HA_LEFT, VA_CENTER);
            
        sendMessageButton_ = rootElement_.CreateChild ("Button", "send_message");
        sendMessageButton_.SetStyleAuto (styles_);
            
        sendMessageButtonText_ = sendMessageButton_.CreateChild ("Text", "text");
        sendMessageButtonText_.text = "Send";
        sendMessageButtonText_.SetStyleAuto (styles_);
        sendMessageButtonText_.SetAlignment (HA_CENTER, VA_CENTER);
        
        for (int index = 0; index < PROPERTY_OTHER_PLAYERS_LABELS_POOL_SIZE; index++)
        {
            Text @label = rootElement_.CreateChild ("Text", "other_player_label_" + index);
            label.text = "...";
            label.SetStyleAuto (styles_);
            label.color = Color (0.3f, 0.8f, 0.3f);
            otherPlayersLabels_.Push (label);
        }
        
        SubscribeToEvent ("UIMouseClickEnd", "HandleUiMouseClickEnd");
        SubscribeToEvent ("NewChatMessage", "HandleNewChatMessage");
        SubscribeToEvent ("NewServerMessage", "HandleNewServerMessage");
    }
    
    void Update (float timeStep)
    {
        if (lastScreenSize.x != graphics.width or lastScreenSize.y != graphics.height)
        {
            lastScreenSize.x = graphics.width;
            lastScreenSize.y = graphics.height;
            ResizeUi (lastScreenSize.x, lastScreenSize.y);
        }
        UpdateChat ();
        UpdatePlayersLabels ();
        HandleKeyboard (timeStep);
    }
    
    void Stop ()
    {
        rootElement_.Remove ();
    }
    
    void ResizeUi (int width, int height)
    {
        float aspectRatio = 1.0f * width / height;
        chatHistoryUI_.SetPosition (height * 0.03f, height * 0.7f);
        chatHistoryUI_.fontSize = height * 0.02f;
            
        sendMessageButton_.SetPosition (width - height * 0.13f, height * 0.93f);
        sendMessageButton_.SetSize (height * 0.1f, height * 0.04f);
        sendMessageButtonText_.fontSize = height * 0.02f;
            
        messageEdit_.SetPosition (width - height * 0.63f, height * 0.93f);
        messageEdit_.SetSize (height * 0.5f, height * 0.04f);
        messageEdit_.textElement.fontSize = height * 0.02f;
    }
    
    void UpdateChat ()
    {
        if (chatHistory_.length == 0)
            chatHistoryUI_.text = "No chat messages yet...";
        else
        {
            chatHistoryUI_.text = "";
            while (chatHistory_.length > 10)
                chatHistory_.Erase (0);
            for (int index = 0; index < chatHistory_.length; index++)
                chatHistoryUI_.text = chatHistoryUI_.text + "\n" + chatHistory_ [index];
        }
    }
    
    void HandleKeyboard (float timeStep)
    {
        if (input.keyPress [PROPERTY_KEY_SWITCH_INPUT_CHAT_MESSAGE])
        {
            messageEdit_.selected = !messageEdit_.selected;
            messageEdit_.focus = !messageEdit_.focus;
        }
            
        if (input.keyPress [PROPERTY_KEY_SEND_CHAT_MESSAGE] and 
            messageEdit_.focus == true)
            SendChatMessage ();
        
        if (messageEdit_.focus == false)
        {
            Vector3 move;
            if (input.keyDown [PROPERTY_KEY_LEFT])
               move.x -= 1;

            if (input.keyDown [PROPERTY_KEY_RIGHT])
                move.x += 1;

            if (input.keyDown [PROPERTY_KEY_FORWARD])
                move.z += 1;
        
            if (input.keyDown [PROPERTY_KEY_BACK])
                move.z -= 1;

            if (input.keyDown [PROPERTY_KEY_UP])
                move.y += 1;

            if (input.keyDown [PROPERTY_KEY_DOWN])
                move.y -= 1;

            if (input.keyDown [PROPERTY_KEY_SPEED_UP])
                move *= PROPERTY_SPEED_UP_MODIFER;

            move *= PROPERTY_DEFAULT_CAMERA_MOVE_SPEED;
            move *= timeStep;
            cameraNode_.position = cameraNode_.position + move;
        }
    }
    
    void UpdatePlayersLabels ()
    {
        Array <Node @> sceneNodes = scene.GetChildren (true);
        Array <Node @> otherPlayersNodes;
        Camera @camera = cameraNode_.GetComponent ("Camera");
        
        for (int index = 0; index < sceneNodes.length; index++)
        {
            Node @processingNode = sceneNodes [index];
            if (processingNode.id < FIRST_LOCAL_ID and
                processingNode.id != node.id and
                processingNode.vars ["ObjectType"].GetInt () ==
                SerializationConstants__OBJECT_TYPE_PLAYER)
                {
                    Vector2 screenPoint = camera.WorldToScreenPoint (processingNode.worldPosition);
                    if (screenPoint.x > 0.0f and screenPoint.x < 1.0f and
                        screenPoint.y > 0.0f and screenPoint.y < 1.0f)
                        otherPlayersNodes.Push (processingNode);
                }
        }
                    
        for (int index = 0; index < otherPlayersLabels_.length; index++)
        {
            Text @label = otherPlayersLabels_ [index];
            if (index < otherPlayersNodes.length)
            {
                Node @otherPlayerNode = otherPlayersNodes [index];
                String labelText = 
                    otherPlayerNode.vars [SerializationConstants__NAME_VAR_HASH].
                        GetString () + "\n";
                    
                if (not otherPlayerNode.HasTag ("Died"))
                {    
                    int otherPlayerLives = Floor (otherPlayerNode.vars 
                                                  [SerializationConstants__HEALTH_VAR_HASH].
                                                  GetFloat ());
                    int otherPlayerExp = otherPlayerNode.vars 
                                         [SerializationConstants__EXP_VAR_HASH].GetInt ();
                    int OtherPlayerMaxLives = Floor (GameplayConstants__BASIC_MAX_HEALTH *
                                              (1.0f + otherPlayerExp * 
                                              GameplayConstants__MAX_HEALTH_INCREASE_PER_EXP));
                        
                    labelText += otherPlayerLives + "/" + OtherPlayerMaxLives + " HP.\n" +
                                 otherPlayerExp + " EXP.";
                }
                else
                    labelText += "[Died]";
                        
                label.text = labelText;
                label.visible = true;
                
                float distance = (cameraNode_.position - otherPlayerNode.position).length;
                Vector2 labelPosition = 
                    camera.WorldToScreenPoint (otherPlayerNode.position + Vector3 (0, 1.5f, 0));
                        
                if (distance > 7.0f)
                {
                    label.fontSize = Floor (graphics.height * 0.1f * 7.0f / distance);
                    labelPosition.x -= Floor (0.08f * 7.0f / distance);
                }
                else
                {
                    label.fontSize = Floor (graphics.height * 0.1f);
                    labelPosition.x -= Floor (0.08f * 7.0f);
                }
                label.SetPosition (graphics.width * labelPosition.x,
                                   graphics.height * labelPosition.y);
            }
            else
                label.visible = false;
        }
        }
    
    void HandleUiMouseClickEnd (StringHash eventType, VariantMap &eventData)
    {
        if (eventData ["Element"].GetPtr () is sendMessageButton_)
            SendChatMessage ();
    }
    
    void SendChatMessage ()
    {
        VariantMap eventData;
        eventData ["Message"] = messageEdit_.text;
        SendEvent ("RequestServerMessage", eventData);
        
        messageEdit_.selected = false;
        messageEdit_.focus = false;
        messageEdit_.text = "";
    }
    
    void HandleNewChatMessage (StringHash eventType, VariantMap &eventData)
    {
        chatHistory_.Push ("[" + time.timeStamp.Substring (11, 8) + " " + 
                           eventData ["Sender"].GetString () + "]" +
                            " : " + eventData ["Message"].GetString ());
    }
    
    void HandleNewServerMessage (StringHash eventType, VariantMap &eventData)
    {
        chatHistory_.Push ("{" + time.timeStamp.Substring (11, 8) +
                           " Server} : " + eventData ["Message"].GetString ());
    }
}
