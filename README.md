Multicast Server документация:

1) Общая концепция приложения:
	Получив permanent AES key размеров 128 бит от центра аутентификации сервер считывает данные из БД Redis(key, field, value) 
   хранящиеся в формате хеш-таблицы, полученным ключом сервер шифрует данные и отправляет multicast рассылкой клиентам.

2) Пошагово
   - Отправка запроса центру аутентификации и при положительной аутентификации получения ключа для шифрования AES CBC Mode 128 bit.
   - Считывание всех данных из БД Redis.
   - Сериализация полученных и зашифрованных данных для отправки по сети.( На данный момент использавался boost::serialization,
	однако, сейчас хочу попробовать кастомную и сравнить в производительности, т.к. скорость библиотечной 
	сериализации не впечатлила, да и зависимость не хочу тащить ).
   - Отправка по сети зашифрованных данных по групповым адресам.

3) Покомпонентно( по названию директорий )
   - authentication( хочу пересмотреть и улучшить ): используя симетричный блочный шифр AES CBC и ассиметричный 
   алгоритм шифрования RSA 2014bit и хеш-функции sha3 256bit проводит аутентификацию у центра аутентификации(ЦА). Данные 
   криптопримитивы взяты из библиотеки crypto++.
  
   - crypto_module: непосредственно реализиция методов для шифрования построенных на основе crypto++.

   - db_driver: модуль работы с БД. Сейчас это Redis, но еще необходимо добавить postgres для логов(аудита) работы сервера.
   Основные функции: коннект к самой БД, считываниевсей информации по заданному ключу и считывание из БД.
  
   - multicast_server: непосредственно сам сервер осуществляющий отправку данных.

   - network: модуль работы с сетью.
     - Unix Sockets: Реализовано как tcp/udp сервер так и клиент. Используется для аутентификации и логов в postgres(в дальнейшем)
     - Multicast: для отправки сообщений по групповым алресам.

   - serialization: выполняет сериализацию зашифрованных данных для отправки.
# MCasrTest
