### SS5Player for Unreal Engine 4

ドキュメントはこちらです。  
https://github.com/SpriteStudio/SS5PlayerForUnrealEngine4/wiki

チュートリアルと機能リファレンスはこちらです。  
http://historia.co.jp/spritestudio


SS5Player for Unreal Engine 4は開発を終了しました。  

現在開発中のSS6Player for Unreal Engine 4はこちらです。  
https://github.com/SpriteStudio/SS6PlayerForUnrealEngine4

SS5Player for Unreal Engine 4の今後のサポート等はUE4のバージョンアップに伴うプラグインのビルド対応を2018/12まで提供する予定です。  

##### 対応UE4バージョン
UE4.21

※ v1.2.17_UE4.21 での仕様変更について
- ComponentでのMix以外のアルファブレンドモードが反映されなくなりました（UE4のアップデートにより、従来の実装が機能しなくなったためです。代替手段を調査していますが、現在対応の目処は立っておりません）
- オフスクリーン描画モードの場合、Mix以外のアルファブレンドモードが反映されるようになりました

※ UE4.17以降では、下記のシェーダファイルのコピーは不要になりました。  

※ UE4.16以前のバージョンでは、導入の際に、シェーダファイルをエンジンのインストールフォルダにコピーする必要がありますので、ご注意下さい  
   v1.1.0_UE4.10_SS5.6.0 でシェーダファイルを更新しました。以前のバージョンから更新される場合は、再度コピーする必要があります。  

※ 旧バージョンのUE4で使用したい場合は、該当のTagから取得して下さい
