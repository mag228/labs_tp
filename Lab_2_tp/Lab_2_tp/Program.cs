using System;
using System.IO;
using System.Xml.Serialization;

namespace Lab_2_tp
{
    [Serializable]
    public class Solute
    {
        private double _volume;
        private double _concentration;

        public double volume { get => _volume; set => _volume = value; }
        public double concentration { get => _concentration; set => _concentration = value; }


        public Solute()
        {
            _volume = _concentration = 0;
        }

        public double getWater()
        {
            return (100 - _concentration) * _volume / 100;
        }

        public double getSubstance()
        {
            return _concentration * _volume / 100;
        }

        public void addWater(double water)
        {
            _concentration = _volume * _concentration / (_volume + water);
            _volume += water;
        }

        public void addSubstance(double substance)
        {
            _concentration = (_volume * _concentration / 100 + substance) * 100 / (_volume + substance);
            _volume += substance;
        }

    }


    class Program
    {
        public static void PrintState(Solute solute)
        {
            Console.WriteLine("Cостояние раствора: ");
            Console.WriteLine("Объем раствора: " + solute.volume + "л");
            Console.WriteLine("Вода:\t " + solute.getWater() + "л - " + (100 - solute.concentration) + "%");
            Console.WriteLine("Примесь: " + solute.getSubstance() + "л - " + solute.concentration + "%");
            Console.WriteLine();
        }

        public static void AddWater(Solute solute, double water)
        {
            solute.addWater(water);
            Console.WriteLine("В раствор было добавлено " + water + "л воды");
        }

        public static void AddSubstance(Solute solute, double substance)
        {
            solute.addSubstance(substance);
            Console.WriteLine("В раствор было добавлено " + substance + "л примеси");
        }

        public static void SaveInXml(Solute solute, string path, XmlSerializer serializer)
        {
            StreamWriter sw = new StreamWriter(path);
            serializer.Serialize(sw, solute);
            sw.Close();
            Console.WriteLine("Состояние было сохранено\n");
        }

        public static Solute LoadFromXml(string path, XmlSerializer serializer)
        {
            FileStream fs = new FileStream(path, FileMode.Open);
            Solute solute = (Solute)serializer.Deserialize(fs);
            fs.Close();
            Console.WriteLine("Состояние было восстановлено");
            return solute;
        }

        public static Solute ChoiseLoad(string path, XmlSerializer serializer)
        {
            Solute solute = new Solute();
            Console.Write("Хотите восстановить состояние по xml (Y/N)? ");
            string answer = Console.ReadLine();
            switch (answer)
            {
                case "Y":
                    try
                    {
                        solute = LoadFromXml(path, serializer);
                    }
                    catch (FileNotFoundException e)
                    {
                        Console.WriteLine("Файл сохранений не найден. Состояние не было восстановлено\n");
                    }
                    break;
                case "N":
                    Console.WriteLine("Состояние не было восстановлено\n");
                    break;
                default:
                    try
                    {
                        Console.WriteLine("Некорректный ответ, но, во избежание потери данных,");
                        solute = LoadFromXml(path, serializer);
                    }
                    catch (FileNotFoundException e)
                    {
                        Console.WriteLine("Состояние не было восстановлено, так как файл сохранений не найден\n");
                    }
                    break;
            }
            return solute;
        }

        static void Main(string[] args)
        {
            Console.WriteLine("Добро пожаловать в единственную в своем роде программу, отображающую состояние смеси!\n");
            string answer;
            string path = "solute.xml";
            double volume;
            XmlSerializer serializer = new XmlSerializer(typeof(Solute));

            Solute solute = ChoiseLoad(path, serializer);

            while (true)
            {
                PrintState(solute);
                Console.WriteLine();
                Console.Write("Введите комманду (help - справка): ");
                answer = Console.ReadLine();
                Console.Clear();

                switch (answer)
                {
                    case "help":
                        Console.WriteLine("Для управления программой предоставлены команды: ");
                        Console.WriteLine("\thelp - показать эту прекрасную справку");
                        Console.WriteLine("\taddWater - добавить в смесь воды");
                        Console.WriteLine("\taddSubstance - добавить в смесь примесь");
                        Console.WriteLine("\texit - выйти из программы");
                        Console.WriteLine();
                        break;
                    case "exit":
                        Console.WriteLine("Всего хорошего! Для продолжения нажмите enter");
                        break;
                    case "addWater":
                        try
                        {
                            Console.Write("Введите объем воды для добавления: ");
                            volume = Double.Parse(Console.ReadLine());
                            if (volume<0) throw new FormatException();
                            AddWater(solute, volume);
                        }
                        catch (FormatException e)
                        {
                            Console.WriteLine("Добавляемый объем должен быть вещественным числом больше нуля!");
                        }
                        break;
                    case "addSubstance":
                        try
                        {
                            Console.Write("Введите объем примеси для добавления: ");
                            volume = Double.Parse(Console.ReadLine());
                            if (volume < 0) throw new FormatException();
                            AddSubstance(solute, volume);
                        }
                        catch (FormatException e)
                        {
                            Console.WriteLine("Добавляемый объем должен быть вещественным числом больше нуля!");
                        }
                        break;
                    default:
                        Console.WriteLine("Некорректный ввод или баг");
                        break;
                }
                SaveInXml(solute, path, serializer);
                if (answer == "exit") break;
            }

            Console.ReadLine();
        }
    }
}
