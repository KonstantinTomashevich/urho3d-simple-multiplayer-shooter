namespace Ingame
{
    // *** Ids for ui buttons
    const int UNKNOWN_BUTTON = -1;
    const int SEND_MESSAGE_BUTTON = 0;
    // ***
    
    // *** Ui constants
    int OTHER_PLAYERS_LABELS_POOL_SIZE = 20;
    int LADDER_LABELS_POOL_SIZE = 5;
    // ***
    
    class StateUi
    {
        // *** Protected variables
        protected IntVector2 lastScreenSize;
        protected XMLFile @styles_;
        protected UIElement @rootElement_;
        
        protected Text @infoText_;
        protected Array <String> chatHistory_;
        protected Text @chatHistoryUI_;
        
        protected Button @sendMessageButton_;
        protected Text @sendMessageButtonText_;
        protected Array <Text @> otherPlayersLabels_;
        
        protected Array <Text @> ladderLabels_;
        protected float timeUntilLadderUpdate_ = 0.0f;
        // ***
        
        // *** Public variables
        LineEdit @messageEdit_;
        String nickname_ = "...";
        bool isSpawned_ = false;
        float timeUntilSpawn_ = 0.0;
        // ***
        
        StateUi ()
        {
            
        }
        
        ~StateUi ()
        {
            
        }
        
        // *** Add all ui elements as childs of specified elements or to ui root if argument not specified.
        void Setup (UIElement @rootElement = null)
        {
            styles_ = cache.GetResource ("XMLFile", "UI/DefaultStyle.xml");
            if (rootElement is null)
                rootElement_ = ui.root;
            else
                rootElement_ = rootElement;
            
            infoText_ = rootElement_.CreateChild ("Text", "info_text");
            infoText_.text = "...\n...";
            infoText_.SetStyleAuto (styles_);
            
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
            
            for (int index = 0; index < OTHER_PLAYERS_LABELS_POOL_SIZE; index++)
            {
                Text @label = rootElement_.CreateChild ("Text", "other_player_label_" + index);
                label.text = "...";
                label.SetStyleAuto (styles_);
                label.color = Color (0.3f, 0.8f, 0.3f);
                otherPlayersLabels_.Push (label);
            }
            
            for (int index = 0; index < LADDER_LABELS_POOL_SIZE; index++)
            {
                Text @label = rootElement_.CreateChild ("Text", "ladder_label_" + index);
                label.text = "...";
                label.SetStyleAuto (styles_);
                label.color = Color (0.9f, 0.9f, 0.9f);
                ladderLabels_.Push (label);
            }
            
            ladderLabels_ [0].color = Color (0.9f, 0.9f, 0.2f);
            ladderLabels_ [1].color = Color (0.6f, 0.6f, 0.6f);
            ladderLabels_ [2].color = Color (0.9f, 0.5f, 0.0f);
        }
        // ***
        
        // *** Calculate and set ui elements positions and sizes
        void Resize (int width, int height)
        {
            float aspectRatio = 1.0f * width / height;
            infoText_.SetPosition (height * 0.03f, height * 0.03f);
            infoText_.fontSize = height * 0.05f;
            
            chatHistoryUI_.SetPosition (height * 0.03f, height * 0.7f);
            chatHistoryUI_.fontSize = height * 0.02f;
            
            sendMessageButton_.SetPosition (width - height * 0.13f, height * 0.93f);
            sendMessageButton_.SetSize (height * 0.1f, height * 0.04f);
            sendMessageButtonText_.fontSize = height * 0.02f;
            
            messageEdit_.SetPosition (width - height * 0.63f, height * 0.93f);
            messageEdit_.SetSize (height * 0.5f, height * 0.04f);
            messageEdit_.textElement.fontSize = height * 0.02f;
            
            for (int index = 0; index < LADDER_LABELS_POOL_SIZE; index++)
            {
                ladderLabels_ [index].SetPosition (width * 0.525f, height * 0.025f * (index + 1));
                ladderLabels_ [index].fontSize = height * 0.0135f;
            }
        }
        // ***
        
        // *** Update ui layout
        void Update (float timeStep)
        {
            if (lastScreenSize.x != graphics.width or lastScreenSize.y != graphics.height)
            {
                lastScreenSize.x = graphics.width;
                lastScreenSize.y = graphics.height;
                Resize (lastScreenSize.x, lastScreenSize.y);
            }
            
            // TODO: Refactor. Maybe split to special functions.
            UpdateInfoText ();
            UpdateChat ();
            UpdatePlayersLabels ();
            UpdateLadderLabels (timeStep);
        }
        
        void UpdateInfoText ()
        {
            String info = nickname_ + "\n";
            if (isSpawned_)
            {
                int lives = Floor (localSceneManager.playerLives);
                int exp = localSceneManager.playerExp;
                int maxLives = Floor (GameplayConstants__BASIC_MAX_HEALTH * 
                                      (1.0f + exp * GameplayConstants__MAX_HEALTH_INCREASE_PER_EXP));
                info += lives + "/" + maxLives + " HP.\n" + exp + " EXP.";
            }
            else
                info += Floor (timeUntilSpawn_) + "s until respawn.";
            infoText_.text = info;
            
            if (!isSpawned_)
                networkHandler.RequestTimeUntilSpawnFromServer ();
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
        
        void UpdatePlayersLabels ()
        {
            Array <Node @> otherPlayersNodes = localSceneManager.otherPlayersNodes;
            Node @cameraNode = localSceneManager.cameraNode;
            Camera @camera = cameraNode.GetComponent ("Camera");
                    
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
                    
                    float distance = (cameraNode.position - otherPlayerNode.position).length;
                    Vector2 labelPosition = 
                        camera.WorldToScreenPoint (otherPlayerNode.position + Vector3 (0, 1.5f, 0));
                        
                    if (distance > 7.0f)
                    {
                        label.fontSize = Floor (graphics.height * 0.04f * 7.0f / distance);
                        labelPosition.x -= Floor (0.08f * 7.0f / distance);
                    }
                    else
                    {
                        label.fontSize = Floor (graphics.height * 0.04f);
                        labelPosition.x -= Floor (0.08f * 7.0f);
                    }
                    label.SetPosition (graphics.width * labelPosition.x,
                                       graphics.height * labelPosition.y);
                }
                else
                    label.visible = false;
            }
        }
        
        void UpdateLadderLabels (float timeStep)
        {
            timeUntilLadderUpdate_ -= timeStep;
            if (timeUntilLadderUpdate_ > 0.0f)
                return;
            
            timeUntilLadderUpdate_ = 2.0f;
            Array <Variant> leaderboard = SharedGlobals::syncedGameScene.vars ["Leaderboard"].GetVariantVector ();
            bool isPlayerFinded_ = false;
            int lastScannedIndex_ = 0;
            
            for (int index = 0; index < LADDER_LABELS_POOL_SIZE; index++)
            {
                lastScannedIndex_ = index;
                if (index < leaderboard.length)
                {
                    String data = leaderboard [index].GetString ();
                    if (data.StartsWith (nickname_) and not isPlayerFinded_)
                        isPlayerFinded_ = true;
                    
                    Array <String> information = data.Split (';');
                    String labelText = (index + 1) + ". " + information [0] + "    ";
                    labelText += "Kills: " + information [1] + "    ";
                    labelText += "Deaths: " + information [2] + "    ";
                    labelText += "EXP: " + information [3] + "    ";
                    labelText += "Summary points: " + information [4];
                    ladderLabels_ [index].text = labelText;
                }
            }
            
            if (not isPlayerFinded_)
            {
                ladderLabels_ [LADDER_LABELS_POOL_SIZE - 2].text = "______________________________________";
                for (int index = lastScannedIndex_; index < leaderboard.length; index++)
                {
                    String data = leaderboard [index].GetString ();
                    if (data.StartsWith (nickname_) and not isPlayerFinded_)
                    {
                        Array <String> information = data.Split (';');
                        String labelText = (index + 1) + ". " + information [0] + "    ";
                        labelText += "Kills: " + information [1] + "    ";
                        labelText += "Deaths: " + information [2] + "    ";
                        labelText += "EXP: " + information [3] + "    ";
                        labelText += "Summary points: " + information [4];
                        ladderLabels_ [LADDER_LABELS_POOL_SIZE - 1].text = labelText;
                    }
                }
            }
        }
        // ***
        
        // *** Removes all ui elements
        void Clear ()
        {
            rootElement_.RemoveAllChildren ();
        }
        // ***
        
        int GetButtonId (Button @button)
        {
            if (button is sendMessageButton_)
                return SEND_MESSAGE_BUTTON;
            else
                return UNKNOWN_BUTTON;
        }
        
        void AddChatMessage (String message)
        {
            chatHistory_.Push (message);
        }
    };
}
